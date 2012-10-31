#include "altgmp.h"
#include "utility.h"
#include "fft.h"

void h_mul_d(hfloat* ha, double b, hfloat* hr) { hfloat hb; h_init(&hb); h_copy_d(&hb, b); h_mul(ha, &hb, hr); h_clear(&hb);}

// разбивает мантиссы на части: 239f a091 -> 2.0, 3.0, 9.0, f.0, a.0, 0.0, 9.0, 1.0
// разбивка идет на группы по n битов
void _split_mant(unsigned __int32* m, double* d, unsigned char n)
{
	unsigned __int32 limb,dlimb;
	unsigned int i,j,k,di;
	// k - количество выделенных битов в double
	// j - количество выделенных битов из конкретного лимба
	// i - index текущего лимба
	// di- индекс текущего double
	dlimb=0;
	i=j=di=k=0;
	while(i<MSIZE)
	{
		j=0;
		limb = m[i];
		while(j<32)
		{
			if(limb&0x80000000) dlimb|=0x00000001;
			k++; if(k==n) { d[di]=(double)dlimb; di++; dlimb=0; k=0;}
			dlimb<<=1; limb<<=1; j++;
		}
		i++;
	}
}

unsigned __int32 _get_real_length(hfloat* a)
{
	int i=getmsize()-1; while(a->m[i]==0) i--;
	return i+1;
}

// умножение сверткой Фурье
void _fft_mul(hfloat* ha, hfloat* hb, hfloat* hr)
{
	double  *amd, *bmd, *rmd;
	complex *aft, *bft, *rft;
	double d;
	unsigned int max_part;
	unsigned int i,j,k, sampsize;
	// ДОДЕЛАТЬ ВЫЧИСЛЕНИЕ ОПТИМАЛЬНОЙ ДЛИНЫ СВЕРТКИ

	//_get_real_length(ha);
	//_get_real_length(hb);

	sampsize = MSIZE*32/PARTSIZE*2;
	max_part=1<<PARTSIZE;
	// переводим мантиссы из 32-bit integers в doubles
	// точность ограничена, так что будем разбивать каждый лимб на части (размером частей (PARTSIZE) поварьируем)
	amd = (double*)calloc(sampsize*6, sizeof(double));
	bmd = amd+sampsize;
	rmd = amd+sampsize*2;

	aft = (complex*)malloc(sizeof(complex)*sampsize*3);
	bft = aft+sampsize;
	rft = aft+sampsize*2;

	_split_mant(ha->m, amd, PARTSIZE);
	_split_mant(hb->m, bmd, PARTSIZE);

	forward_fft(amd, aft, sampsize);
	forward_fft(bmd, bft, sampsize);

	for(i=0;i<sampsize;i++)
	{
		//rft[i] = aft[i]*bft[i];
		rft[i].re = aft[i].re * bft[i].re - aft[i].im * bft[i].im;
		rft[i].im = aft[i].re * bft[i].im + aft[i].im * bft[i].re;
	}

	inverse_fft(rft, rmd, sampsize);

	// нормализуем коэффициенты
	d=0;
	for(i=sampsize;i>0;i--)
	{
		rmd[i-1]=(unsigned __int32)(rmd[i-1]+0.1);
		rmd[i-1]+=d;
		d=(unsigned int)rmd[i-1]/max_part;
		rmd[i-1]-=d*max_part;
	}

	// делаем нормальную мантиссу из double
	unsigned __int32 limb;
	limb = (unsigned __int32)d;
	k=PARTSIZE; // k - количество битов, загнанных в лимб
	j=0; // j - индекс double-коэффициента
	for(i=0;i<MSIZE;i++)
	{
		while(k<32)
		{
			limb<<=PARTSIZE;
			limb|=(unsigned __int32)rmd[j];
			j++; k+=PARTSIZE;
		}
		hr->m[i]=limb; 
		limb=0L;
		k=0;
	}

	// ХАЛЯВА: порядки ...
	hr->e = ha->e + hb->e;

	// ...нормализация...
	h_norm(hr);

	// ... и знаки
	hr->s = ha->s ^ hb->s;
	
	free(aft);
	free(amd);
}

//#define MUL_A
#define MUL_2A
//#define MUL_DW

void h_mul(hfloat* ha, hfloat* hb, hfloat* hr)
{
	unsigned int i;
	unsigned int la,lb;
	//unsigned int lmin,lmax;

	if( (h_iszero(ha)) || (h_iszero(hb)) ) { h_copy_d(hr, 0.0); return; }
	
	la=_get_real_length(ha);
	lb=_get_real_length(hb);
	//lmax = max(la,lb);
	//lmin = min(la,lb);
	if((la>=4)&&(lb>=4)) {_fft_mul(ha,hb,hr); return;}

#if defined(MUL_A)||defined(MUL_2A)
	hfloat a; h_init(&a);
	hfloat b; h_init(&b);

	i=MSIZE-1;
	while(i>=0)
	{
		if(ha->m[i]!=0)
		{
			h_copy(&a, ha);
			h_copy(&b, hb);
			break;
		}
		if(hb->m[i]!=0)
		{
			h_copy(&a, hb);
			h_copy(&b, ha);
			break;
		}
		i--;
	}

	h_mshr(&a,1); a.e++;
	h_mshr(&b,1); b.e++;
#endif

	// перемножаем мантиссы
	unsigned __int32 *MLP=(unsigned __int32*)calloc(4, MSIZE*2); // для произведения мантисс
#if defined(MUL_DW)
	unsigned __int32 *am = ha->m;
	unsigned __int32 *bm = hb->m;
	unsigned __int32 *c0 = (unsigned __int32*)calloc(4,MSIZE);
	unsigned __int32 *c1 = (unsigned __int32*)calloc(4,MSIZE);
	unsigned __int32 _bl;
	__asm {
		mov edi, bm
		add edi, LMSIZE
		mov ecx, MSIZE
next_b:
		push edi ; в [edi] - очередные слова числа b
		push ecx
		mov eax, [edi]
		mov _bl, eax

		// умножаем все слова числа a на _bl
		mov ecx, MSIZE

		mov edi, am
		add edi, LMSIZE
		mov esi, c1
		add esi, LMSIZE
		mov ebx, c0
		add ebx, LMSIZE
mul_a_ebx:
		mov eax, [edi]
		mul _bl
		mov [esi], edx
		mov [ebx], eax
		sub esi,4
		sub edx,4
		sub edi,4
		loop mul_a_ebx

// сдвигаем r вправо... 
// (вообще-то можно сделать без всяких сдвигов)

		mov esi, MLP
		mov ecx, MSIZE
		clc
		rcl ecx, 3
		sub ecx, 4
		add esi, ecx
		mov edi, esi
		sub esi, 4
		mov ecx, MSIZE
		clc
		rcl ecx, 1
		dec ecx
		std
		rep movsd
		cld
		mov dword ptr [edi],0

// теперь в с0,с1 - произведение слова из b на a
// сводим воедино - складываем: r[i] = r[i] + c1[i] + c0[i+1]
		// r = r + c0
		mov edi, MLP
		mov esi, c0

		add edi, LMSIZE
		add edi, 4
		add esi, LMSIZE
		mov ecx, MSIZE
		clc
		lahf
add_r_c0:
		sahf
		mov eax, [edi]
		adc eax, [esi]
		mov [edi], eax
		lahf
		dec esi
		dec esi
		dec esi
		dec esi
		dec edi
		dec edi
		dec edi
		dec edi
		loop add_r_c0

		// r = r + c1<<32
		mov edi, MLP
		mov esi, c1

		add edi, LMSIZE
		add esi, LMSIZE
		mov ecx, MSIZE
		clc
		lahf
add_r_c1:
		sahf
		mov eax, [edi]
		adc eax, [esi]
		mov [edi], eax
		lahf
		dec esi
		dec esi
		dec esi
		dec esi
		dec edi
		dec edi
		dec edi
		dec edi
		loop add_r_c1
// теперь в r - сумма r+c0+c1<<32

		pop ecx
		pop edi
		sub edi,4
		dec ecx
		jnz next_b
	}
	free(c0);
	free(c1);
#endif

#if defined(MUL_2A)
	unsigned __int32 *a2 =(unsigned __int32*)malloc(MSIZE*2*4);
	unsigned __int32 *a3 = a2+MSIZE;
	h_mshr(&a,1);
	h_mshr(&b,1);

// умножение по алгоритму 2A
	__asm {
		// генерируем 2А
		mov esi, a.m
		mov edi, a2
		mov ecx, MSIZE
		cld
		rep movsd
		mov ebx, a2
		call _mshl 

		// генерируем 3A
		mov edi, a3
		mov ebx, a2
		mov esi, a.m
		call _madd

		mov edx, b.m
		add edx, LMSIZE
		mov ecx, MSIZE
skipzero:
		mov eax, [edx]
		or eax,eax
		jnz mul2A
		dec ecx
		sub edx,4
		jmp skipzero
mul2A:
		push ecx
		mov ecx, 16
		mov eax, [edx]
		push edx
test2:
		push ecx
		rcr eax,1
		jnc is_X0
		// X1
		rcr eax,1
		jnc is_01
		// 11 - прибавляем 3A
		mov edi, MLP
		mov ebx, edi
		mov esi, a3
		push eax
		call _madd
		pop eax
		jmp cont
		// 01 - прибавляем A
is_01:
		mov edi, MLP
		mov ebx, edi
		mov esi, a.m
		push eax
		call _madd
		pop eax
		jmp cont
is_X0:
		rcr eax,1
		jnc cont; 00 - пропускаем два бита
		// 10 - прибавляем 2A
		mov edi, MLP
		mov ebx, edi
		mov esi, a2
		push eax
		call _madd
		pop eax
cont:
		mov ebx, MLP
		call _mshr
		mov ebx, MLP
		call _mshr
		pop ecx
		loop test2
		pop edx
		pop ecx
		sub edx, 4
		dec ecx
		jnz mul2A
	}
	//a.m_shl(1); b.m_shl(1);
	free(a2);
#endif
#if defined(MUL_A)
// умножение по алгоритму A
	__asm {
		mov edx, a.m
		add edx, LMSIZE
		mov ecx, MSIZE
main:
		push ecx
		mov eax, [edx]
		mov ecx,32
mlimb:
		push ecx
		// сдвигаем результат вправо
		mov ecx, MSIZE
		rol ecx,1
		mov ebx, MLP
		xor esi,esi
		clc
shright:
		rcr dword ptr [ebx+esi*4],1
		inc esi
		loop shright

		test eax, 1
		push eax
		jz skipbit
		// прибавляем к результату мантиссу b
		mov ebx, MLP
		add ebx, LMSIZE
		mov esi, b.m
		add esi, LMSIZE
		mov edi, MLP
		add edi, LMSIZE
		mov ecx, MSIZE
		clc
		lahf
ad:
		sahf
		mov eax, [ebx]
		adc eax, [esi]
		mov [edi], eax
		lahf
		sub ebx,4
		sub esi,4
		sub edi,4
		loop ad
skipbit:
		pop eax
		pop ecx
		clc
		rcr eax,1
		loop mlimb
		pop ecx
		sub edx,4
		loop main
	}
#endif // (MUL_A)

#if defined (MUL_DW)
	hr->e = ha->e + hb->e;
#endif

#if defined(MUL_2A)
	hr->e = a.e + b.e;
	hr->e+=2;
#endif

#if defined(MUL_A)
	hr->e = a.e + b.e;
#endif

	for(i=0;i<MSIZE;i++) 
		hr->m[i] = MLP[i];
	// нормализуем результат
	h_norm(hr);

	hr->s = ha->s ^ hb->s;

#if defined(MUL_A)||defined(MUL_2A)
	h_clear(&a); h_clear(&b);
#endif
	free(MLP);
}
