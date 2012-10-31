#include "altgmp.h"

void h_clfrac(hfloat* ha)
{
	unsigned int i,k;
	unsigned __int32 mask=0xFFFFFFFF;
	if(ha->e<=0) { h_copy_d(ha, 0.0); return; }
	k=(ha->e)%32;
	i=(ha->e)/32;
	if(i>MSIZE) return;
	mask=mask<<(32-k);
	ha->m[i]&=mask;
	for(i++;i<MSIZE;i++) ha->m[i]=0;
}

void h_clrat(hfloat* ha)
{
	if(ha->e<=0) { return; }
	h_mshl(ha,ha->e);
	ha->e=0;
	h_norm(ha);
}

void h_norm(hfloat* ha) { if(!h_iszero(ha)) while((ha->m[0]&0x80000000)==0) { h_mshl(ha, 1); ha->e--; } }; // нормализует число

bool h_iszero(hfloat* ha)
{
	unsigned int i;
	for(i=0;i<MSIZE;i++) if(ha->m[i]!=0) return false;
	return true;

}
