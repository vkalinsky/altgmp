#include "altgmp.h"
#include "utility.h"
#include "fft.h"

void h_init(hfloat* ha)
{
	ha->s=0;
	ha->e=0;
	ha->m=(unsigned __int32*)malloc(MSIZE*4);
	for(unsigned int i=0;i<MSIZE;i++) ha->m[i]=0;
}

void h_clear(hfloat* ha)
{
	free(ha->m);
}

void h_copy_d(hfloat* dest, double src)
{
	if(src!=0.0)
	{
		// знак
		dest->s=src<0?1:0;
		// порядок
		__int64 *d=(__int64*)&src;
		dest->e = (__int32)((*d & 0x7FF0000000000000)>>52);
		dest->e = dest->e - 1023 + 1;
		// мантисса
		memset(dest->m,0,MSIZE*4);
		int c=0;
		for(int i=1;i>=0;i--) { 
			dest->m[c]=*(((__int32*)&src)+i); c++; 
		}
		void *p = (void*)dest->m;
		h_mshl(dest, 11);
		dest->m[0]|=0x80000000;
	} else { // ноль - специальный случай
		dest->s=0;
		memset(dest->m,0,MSIZE*4);
		dest->e=0;
	}
}

void h_copy(hfloat* dest, hfloat* src) 
{ 
	dest->s = src->s;
	dest->e = src->e;
	for(unsigned int i=0;i<MSIZE;i++) dest->m[i] = src->m[i];
}

double h_getdouble(hfloat* src)
{
	if(h_iszero(src)) return 0;
	__int64 dbl=0;
	__int64 exp=0;
	double rd;
	int c=0; 
	for(int i=1;i>=0;i--) 
	{ 
		*(((__int32*)&dbl)+i)=src->m[c]; c++; 
	}
	dbl>>=11;
	dbl&=0x000FFFFFFFFFFFFF;
	dbl|= src->s==1?0x8000000000000000:0;
	exp = src->e + 1023 - 1; // ???????????
	dbl|= ( exp & 0x000007FF )<<52;
	rd=*((double*)&dbl);
	return rd;
}
