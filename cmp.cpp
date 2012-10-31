#include "altgmp.h"

int h_cmp(hfloat *ha, hfloat* hb) // сравнение чисел: -1: а>b, 0: a==b, 1: a<b
{
	// сравниваем знаки
	if(ha->s > hb->s) return 1;
	if(ha->s < hb->s) return -1;
	// если знаки одинаковые
	return -h_cmpm(ha,hb)* (ha->s*2-1);

}

int h_cmp_d(hfloat *ha, double b)
{
	hfloat hb; h_init(&hb); h_copy_d(&hb, b); 
	int ret=h_cmp(ha, &hb); 
	h_clear(&hb); 
	return ret;
}

int h_cmpm(hfloat *ha, hfloat* hb) // сравнение модулей: так-же, как в cmp(...)
{
	// сравниваем порядки
	__int32 ss = hb->e - ha->e;
	if(ss<0) return -1;
	if(ss>0) return 1;
	// если порядки равны, сравниваем мантиссы
	for(unsigned int i=0;i<getmsize();i++)
	{
		if(ha->m[i] > hb->m[i]) return -1;
		if(ha->m[i] < hb->m[i]) return 1;
	}
	// мантиссы равны, порядки равны => числа равны
	return 0;
}
