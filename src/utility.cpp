#include "altgmp.h"
#include "utility.h"
#include "fft.h"

unsigned __int32 MSIZE=_MSIZE;
unsigned __int32 LMSIZE=(MSIZE-1)*4;

void jumo_init(unsigned __int32 s)
{
	MSIZE=s;
	LMSIZE=(s-1)*4;
	fft_init(MSIZE*32/PARTSIZE*2);
}

void jumo_deinit()
{
	fft_deinit();
}

inline const unsigned __int32 getmsize()
{
	return MSIZE;
}

void h_mneg(hfloat* ha) // инвертирует мантиссу, прибавл€ет 1 (делает отрицательной в ƒ.ќ. )
{
	unsigned int pt=(unsigned int)ha->m;
	__asm {
		mov ecx, MSIZE
		mov ebx, pt
		mov esi, MSIZE
		dec esi
		push esi
ng:
		not dword ptr [ebx+esi*4]
		dec esi
		loop ng

		pop esi
		mov ecx, MSIZE
		dec ecx
		clc
		adc dword ptr [ebx+esi*4],1
		dec esi
ad:
		adc	dword ptr [ebx+esi*4], 0
		dec esi
		loop ad
	}
}

void h_mshl(hfloat* ha, unsigned int count) // сдвиг мантиссы влево
{
	if(count==0) return;
	unsigned int pt = (unsigned int)ha->m;
	__asm {
		mov edx, count
cnt:
		mov ebx, pt
		mov esi, MSIZE
		mov ecx, MSIZE
		dec esi
		clc
sh:
		mov eax,[ebx+esi*4]
		rcl eax,1
		mov [ebx+esi*4],eax
		dec esi
		loop sh
		dec edx
		jnz cnt
	}
}

void h_mshr(hfloat* ha, unsigned int count) // сдвиг мантиссы вправо
{
	if(count==0) return;
	unsigned int pt = (unsigned int)ha->m;
	__asm {
		mov edx, count
cnt:
		mov ebx, pt
		mov esi, 0
		mov ecx, MSIZE
		clc
sh:
		mov eax,[ebx+esi*4]
		rcr eax,1
		mov [ebx+esi*4],eax
		inc esi
		loop sh
		dec edx
		jnz cnt
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// библиотечка быстрых naked - функций дл€ работы с мантиссами, дл€ перевода чисел, проч.
// TODO: заменить этими функци€ми встроенные функции в делении

// сдвиг мантиссы влево
// в ebx - адрес мантиссы
void __declspec(naked) _mshl()
{
	__asm {
		mov esi, MSIZE
		mov ecx, esi
		dec esi
		clc
sl:
		rcl dword ptr [ebx+esi*4],1
		dec esi
		dec ecx
		jnz sl
		ret
	}
}

// сдвиг мантиссы вправо
// в ebx - адрес мантиссы
void __declspec(naked) _mshr()
{
	__asm {
		xor esi,esi
		mov ecx, MSIZE
		clc
sr:
		rcr dword ptr [ebx+esi*4],1
		inc esi
		dec ecx
		jnz sr
		ret
	}
}

// сложение мантисс в доп.обратном коде 
// *ebx + *esi = *edi
void __declspec(naked) _madd()
{
	__asm {
		add ebx, LMSIZE
		add esi, LMSIZE
		add edi, LMSIZE
		mov ecx, MSIZE
		clc
		lahf
adm:
		sahf
		mov eax, [ebx]
		adc eax, [esi]
		mov [edi], eax
		lahf
		sub ebx,4
		sub esi,4
		sub edi,4
		dec ecx
		jnz adm
		ret
	}
}

// переводит число в дес€тичный вид
// eax - число
// esi - адрес строкового буфера дл€ числа
// ch : 7 бит - писать ли незначащие нули в начале: 0 - не писать, >1 - писать
//      0..2 биты - позици€ точки (после какого знака еЄ ставить)
// на выходе в ebx - количество символов, записанных в буфер
void __declspec(naked) ascii4()
{
	__asm {
		xor edx,edx
		mov ebx, 0
		mov cl, 0
		
		test ch, 0x80
		jnz get10000

		cmp ax, 10
		jb m1
		cmp ax, 100
		jb get10
		cmp ax, 1000
		jb get100
		cmp ax, 10000
		jb get1000

get10000:
		and ch, 0x07 ; теперь в ch - позици€ точки
		cmp eax, 10000
		jb m10000
		inc cl
		sub eax, 10000
		jmp get10000
m10000:
		add cl, '0'
		mov [esi+ebx], cl
		inc ebx
		dec ch
		jnz nopoint4
		mov [esi+ebx], '.'
		inc ebx
nopoint4:
		mov cl, 0

get1000:
		cmp eax, 1000
		jb m1000
		inc cl
		sub eax, 1000
		jmp get1000
m1000:
		add cl, '0'
		mov [esi+ebx], cl
		inc ebx
		dec ch
		jnz nopoint3
		mov [esi+ebx], '.'
		inc ebx
nopoint3:
		mov cl, 0
get100:
		cmp eax, 100
		jb m100
		inc cl
		sub eax, 100
		jmp get100
m100:
		add cl, '0'
		mov [esi+ebx], cl
		inc ebx
		dec ch
		jnz nopoint2
		mov [esi+ebx], '.'
		inc ebx
nopoint2:

		mov cl, 0
get10:
		cmp eax, 10
		jb m10
		inc cl
		sub eax, 10
		jmp get10
m10:
		add cl, '0'
		mov [esi+ebx], cl
		inc ebx
		dec ch
		jnz nopoint1
		mov [esi+ebx], '.'
		inc ebx
nopoint1:

m1:
		add al, '0'
		mov [esi+ebx], al
		inc ebx
		dec ch
		jnz nopoint0
		mov [esi+ebx], '.'
		inc ebx
nopoint0:

		mov [esi+ebx], 0
		ret
	}
}
