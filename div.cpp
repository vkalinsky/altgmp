#include "altgmp.h"
#include "utility.h"

void h_div_d(hfloat* ha, double b, hfloat* hr) { hfloat hb; h_init(&hb); h_copy_d(&hb, b); h_div(ha, &hb, hr); h_clear(&hb);}

void h_div(hfloat* ha, hfloat* hb, hfloat* hr)
{
	// у нас работают только мантиссы, их и объявляем
	unsigned __int32* a; // this
	unsigned __int32* b; // other
	unsigned __int32* mb; // -other (в доп.обратном коде)
	unsigned __int32* c; // частное
	unsigned __int32* q; // остаток

	a=(unsigned __int32*)malloc(MSIZE*4);
	b=(unsigned __int32*)malloc(MSIZE*4);
	mb=(unsigned __int32*)malloc(MSIZE*4);
	c=(unsigned __int32*)malloc(MSIZE*4);
	q=(unsigned __int32*)malloc(MSIZE*4);

	memset(q,0,MSIZE*4);
	memset(c,0,MSIZE*4);
	memcpy(a, ha->m, MSIZE*4);
	memcpy(b, hb->m, MSIZE*4);
	memcpy(mb, hb->m, MSIZE*4);

	// сдвигаем все мантиссы вправо
	__asm {
		mov ebx, a
		call _mshr
		mov ebx, b
		call _mshr
		mov ebx, mb
		call _mshr
	}
	// переводим mb в дополнительный обратный код (mb=-b)
	__asm {
		mov ecx, MSIZE
		mov ebx, mb
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
		lahf
		dec esi
ad:
		sahf
		adc	dword ptr [ebx+esi*4], 0
		lahf
		dec esi
		loop ad
	};
	// 1: вычисляем Q0 = A + (-|B|) и сдвигаем Q0 влево
	__asm {
		mov ebx, a
		mov esi, mb
		mov edi, q
		call _madd
		mov ecx, MSIZE
		rol ecx, 5
	}

div_cycle:
	// 2: сдвигаем результат влево
	__asm {
		push ecx
		mov ebx, c
		call _mshl
	}
	// 3: если старший разряд q =1, то пишем в результат 0, q=q+|B|,
	//                           иначе пишем в результат 1, q=q+ (-|B|)
	__asm {
		mov edi, q
		mov ebx, b
		mov esi, q
		test dword ptr [esi],0x80000000
		jnz toff
		mov ebx, c
		add ebx, LMSIZE
		or dword ptr [ebx],1
		mov ebx, mb		;q=q+ (-|B|)
toff:
		push ebx
		push esi
		mov ebx, q
		call _mshl
		pop esi
		pop ebx
		call _madd
		pop ecx
		loop div_cycle
	}
	memcpy(hr->m, c, MSIZE*4);
	hr->e = ha->e - hb->e+1;
	hr->s = ha->s ^ hb->s;
	// нормализуем...
	h_norm(hr);

	free(a);
	free(b);
	free(mb);
	free(c);
	free(q);
}
