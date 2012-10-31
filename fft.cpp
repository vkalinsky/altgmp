#include "fft.h"
#include "utility.h"

unsigned int COMPLEX_COUNT=0;
unsigned int LOG2SIZE; // log2(sampsize)
complex* WROOT;

inline void __stdcall complex_add(complex* a, complex* b, complex* r) 
{ r->re = a->re + b->re; r->im = a->im + b->im; }

inline void __stdcall complex_sub(complex* a, complex* b, complex* r)
{ r->re = a->re - b->re; r->im = a->im - b->im; }

inline void __stdcall complex_mul(complex* a, complex* b, complex* r)
{ double re,im; re = a->re*b->re - a->im*b->im; im = a->re*b->im + a->im*b->re; r->re=re; r->im=im; }

inline void __stdcall complex_copy(complex* tgt, complex* src)
{ tgt->re = src->re; tgt->im = src->im; }


void fft_init(unsigned int sampsize)
{
	_asm {
		mov ebx, sampsize
		xor ecx,ecx
		clc
getlnsize:
		rcr ebx,1
		inc ecx
		jnc getlnsize
		dec ecx
		mov LOG2SIZE, ecx
	}
	// создаем таблицу корней из 1
	WROOT = (complex*)malloc(sizeof(complex)*sampsize);
	for(int i=0;i<sampsize;i++)
	{
		WROOT[i].re = cos(twoPI/sampsize*i);
		WROOT[i].im = sin(twoPI/sampsize*i);
	}
}

void fft_deinit()
{
	free(WROOT);
}

unsigned int __declspec(naked)_rev32(unsigned int n) // "разворачивает число"
{
	__asm {
		xor eax,eax
		mov edx, dword ptr [esp+4]
		mov ecx, LOG2SIZE
		clc
rev:
		rcr edx,1
		rcl eax,1
		dec ecx
		jnz rev
		ret
	}
}

// быстрое преобразование Фурье
void forward_fft(double* s, complex* ft, unsigned int sampsize)
{
	unsigned int i,j,k,l;
	// сначала переставляем элементы массива и представляем их как комплексные числа
	for(i=0;i<sampsize;i++)
	{
		// создаем реверс числа i
		j=_rev32(i);
		// переставляем элементы
		ft[j].re=s[i];
		ft[j].im=0;
	}

	// собственно БПФ
	complex tr,tl;
	for(i=1; i<sampsize; i*=2) // i - размер объединяемых участков
	{
		for(j=0;j<sampsize;j+=i*2) // пробегаем все участки
		{
			for(k=0;k<i;k++) // по участку
			{
				complex_copy(&tl, &ft[j+k]);
				complex_copy(&tr, &ft[j+k+i]);
				complex_mul(&tr, &WROOT[sampsize/i/2*k], &tr);
				complex_add(&tl, &tr, &ft[j+k]);
				complex_sub(&tl, &tr, &ft[j+k+i]);
				/*
				tl = ft[j+k];
				tr = ft[j+k+i];
				tr = tr * WROOT[sampsize/i/2*k];
				ft[j+k]   = tl + tr;
				ft[j+k+i] = tl - tr;
				*/
			}
		}
	}
}

void inverse_fft(complex* ft, double *s, unsigned int sampsize)
{
	unsigned int i,j,k,l;
	complex cm;
	complex *samp = (complex*)malloc(sampsize*sizeof(complex));
	
	// сначала переставляем элементы массива
	for(i=0;i<sampsize;i++)
	{
		// переставляем элементы
		j=_rev32(i);
		complex_copy(&samp[j], &ft[i]);
	}
	  
	// собственно БПФ
	complex tr,tl;
	for(i=1; i<sampsize; i*=2) // i - размер объединяемых участков
	{
		for(j=0;j<sampsize;j+=i*2) // пробегаем все участки
		{
			for(k=0;k<i;k++) // по участку
			{
				complex_copy(&tl, &samp[j+k]);
				complex_copy(&tr, &samp[j+k+i]);
				if(k!=0) complex_mul(&tr, &WROOT[sampsize-sampsize/i/2*k], &tr); 
				else complex_mul(&tr, &WROOT[0], &tr);
				complex_add(&tl, &tr, &samp[j+k]);
				complex_sub(&tl, &tr, &samp[j+k+i]);
				/*
				tl = samp[j+k];
				tr = samp[j+k+i];
				if(k!=0) tr = tr * WROOT[sampsize-sampsize/i/2*k]; else tr = tr * WROOT[0];
				samp[j+k]   = tl + tr;
				samp[j+k+i] = tl - tr;
				*/
			}
		}
	}
	
	for(i=0;i<sampsize;i++)
	{
		s[i] = samp[i].re/sampsize;
	}
	free(samp);
}
