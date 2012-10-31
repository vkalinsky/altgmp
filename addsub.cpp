#include "altgmp.h"
#include "utility.h"

void h_add_d(hfloat* ha, double b, hfloat* hr) { hfloat hb; h_init(&hb); h_copy_d(&hb, b); h_add(ha, &hb, hr); h_clear(&hb);}

void h_add(hfloat* ha, hfloat* hb, hfloat* hr)
{
	if(h_iszero(ha)) { h_copy(hr, hb); return; }
	if(h_iszero(hb)) { h_copy(hr, ha); return; }
	hfloat me; h_init(&me); h_copy(&me, ha);
	hfloat other; h_init(&other); h_copy(&other, hb);

	// выравниваем порядки
	__int32 diff = me.e - other.e;
	if(diff<0) { h_mshr(&me, -diff); me.e+=-diff; } else { h_mshr(&other, diff); other.e+=diff; }
	h_mshr(&me, 1); me.e++;
	h_mshr(&other, 1); other.e++;

	// разбираемся со знаками
	// если знаки разные, то инвертируем мантиссу отрицательного числа,
	// если одинаковые, то оставляем мантиссы без изменений и в результате оставляем знак операндов
	bool isnegresult=false;
	if(me.s!=other.s) {
		if(h_cmpm(ha, hb) == (1-me.s*2)) isnegresult=true;
		if(other.s==1) h_mneg(&other);
		if(me.s==1) h_mneg(&me);
	}

	h_copy_d(hr, 0.0);

	unsigned int pt=(unsigned int)me.m;
	unsigned int o_pt=(unsigned int)other.m;
	unsigned int r_pt=(unsigned int)hr->m;
	__asm {
		mov ebx, pt
		add ebx, LMSIZE
		mov esi, o_pt
		add esi, LMSIZE
		mov edi, r_pt
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
	}

	// устанавливаем знаки
	if( (me.s == other.s) ) hr->s = me.s;
	else { // если знаки разные
		if( isnegresult ) { h_mneg(hr); hr->s = 1; }
	}

	hr->e = me.e;
	// нормализуем результат
	h_norm(hr);
	h_clear(&me); h_clear(&other);
}

void h_sub_d(hfloat* ha, double b, hfloat* hr) { hfloat hb; h_init(&hb); h_copy_d(&hb, b); h_sub(ha, &hb, hr); h_clear(&hb);}

void h_sub(hfloat* ha, hfloat* hb, hfloat* hr)
{
	h_neg(hb);
	h_add(ha,hb,hr);
	h_neg(hb);
}

void h_neg(hfloat* ha) { ha->s = ha->s==0?1:0; }
