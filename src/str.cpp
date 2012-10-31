#include "altgmp.h"
#include "utility.h"

void h_htoa(hfloat* ha, char* buf, unsigned int ccmax, int radix)
{
	if(h_iszero(ha)) { buf[0]='0'; buf[1]='\0'; return; }
	const char abc[]="0123456789ABCDEF";
	hfloat t; h_init(&t); h_copy(&t, ha);

	int i,j,k;
	int exp = 0;
	bool negexp=false;
	__int8 digit=0;
	unsigned __int32 limb;
	short digitsize;
	char* pc=buf;

	memset(buf,0,ccmax);
	if(ha->s) { *pc='-'; pc++; }
	//if(t<1) { *pc='0'; pc++; }
	
	if(radix==16) digitsize=4;
	if(radix==8) digitsize=3;
	if(radix==2) digitsize=1;

	switch(radix)
	{
	case 16:
	case 8:
	case 2:
		exp = ha->e;
		if(exp<=0) 
		{
			negexp=true;
			*pc='0'; pc++; *pc='.'; pc++;
			exp=-exp;
			if(exp>=digitsize) for(i=0;i<exp;i+=digitsize) {*pc='0'; pc++; }
		}
		digit=0;
		if(negexp) k=exp%digitsize;
		else k=digitsize-exp%digitsize;
		if(k==digitsize) k=0;
		for(i=0;i<MSIZE;i++)
		{
			limb = ha->m[i];
			for(j=0;j<32;j++)
			{
				if(limb&0x80000000) digit|=1;
				k++;
				if(k>=digitsize)
				{
					k=0;
					*pc = abc[digit];
					*pc++;
					digit=0;
				}
				exp--; if((exp==0) && !negexp) { *pc='.'; pc++; }
				digit<<=1;
				limb<<=1;
			}
		}
		break;
	case 10:
		hfloat a,b; h_init(&a); h_init(&b);
		h_copy(&t, ha);
		if(t.s!=0) t.s=0;
		exp = 0;
		h_copy_d(&b, 0.1);
		if(t.e<=0) {
			strcpy(pc,"0."); pc+=2;
			while(h_cmp(&t,&b)>0) {
				h_mul_d(&t, 10.0, &t);
				*pc='0'; pc++;
			}
		} else { strcpy(pc, "0"); pc++; } // в начале - 0, для округления с переполнением (999.99999)
		while(h_cmp_d(&t, 1000000000.0)<=0) { h_div_d(&t, 1000000000.0, &t); exp+=9; }
		while(h_cmp_d(&t, 1000000.0)<=0) { h_div_d(&t, 1000000.0, &t); exp+=6; }
		while(h_cmp_d(&t, 1000.0)<=0) 
		{ 
			h_div_d(&t, 1000.0, &t); 
			exp+=3; 
		}
		while(h_cmp_d(&t, 1.0)<=0) { h_div_d(&t, 10.0, &t); exp++; }

		h_clear(&a); h_clear(&b);
		unsigned __int8 _e;
		for(i=0;i<MSIZE*32/4/5;i++)
		{
			h_mul_d(&t,100000.0, &t);
			if( (exp<=5)&&(exp>=0) ) _e=(unsigned __int8)exp; else _e=0;
			exp-=5;
			__asm {
				mov ebx, t.m
				mov eax, [ebx]
				mov ecx, 32
				sub ecx, t.e
getfrac:
				clc
				rcr eax, 1
				dec cl
				jnz getfrac

				mov esi, pc
				mov ch, _e
				or ch, 0x80
				call ascii4
				add esi, ebx
				mov pc, esi
			}
			h_clrat(&t);
		}
		
		// теперь buf содержит наше число в десятичном виде.
		// округляем его (23.01299999999 -> 23.013)
		
		pc = buf+strlen(buf)-1;
		if(*pc=='9') {
			k=1;
			do {
				if(!isdigit(*pc)) { pc--; continue; }
				*pc+=k; if(*pc>'9') { *pc='0'; k=1; } else k=0;
				pc--;
			} while(k==1);
		}
		
		break;
	}
	pc=buf+strlen(buf)-1;
	while( (*pc=='0')&&(pc!=buf) ) { *pc=0; pc--; }
	if(*pc=='.') *pc=0;
	if( (buf[0]=='0')&&(buf[1]!='.') ) memmove(buf, buf+1, strlen(buf));
	if( (buf[0]=='-')&&(buf[1]=='0')&&(buf[2]!='.') ) memmove(buf+1, buf+2, strlen(buf)-1);
	h_clear(&t);
}

unsigned int h_atoh(hfloat* ha, char* buf, int radix)
{
	const char ABC[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	char *pc,c;
	bool havepoint=false;
	int i;
	hfloat r,he,hrad;
	h_init(&r);h_init(&he);h_init(&hrad);
	h_copy_d(&r, 0.0);
	h_copy_d(&he, 1.0);
	h_copy_d(&hrad, radix);

	pc=buf;
	if(*pc=='-') *pc++;
	for(;*pc!=0; pc++)
	{
		if(*pc=='.') { havepoint=true; continue; }
		if(havepoint) h_mul(&he, &hrad, &he);
		h_mul(&r, &hrad, &r);
		c=toupper(*pc);
		for(i=0; (ABC[i]!=c) && (ABC[i]!=0); i++);
		if(ABC[i]==0) return -1;
		h_add_d(&r, (double)i, &r);
	}
	if(havepoint) h_div(&r, &he, &r);
	if(buf[0]=='-') r.s=1;
	h_copy(ha, &r);
	h_clear(&r); h_clear(&he); h_clear(&hrad);
	return 0;
}
