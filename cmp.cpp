#include "altgmp.h"

int h_cmp(hfloat *ha, hfloat* hb) // ��������� �����: -1: �>b, 0: a==b, 1: a<b
{
	// ���������� �����
	if(ha->s > hb->s) return 1;
	if(ha->s < hb->s) return -1;
	// ���� ����� ����������
	return -h_cmpm(ha,hb)* (ha->s*2-1);

}

int h_cmp_d(hfloat *ha, double b)
{
	hfloat hb; h_init(&hb); h_copy_d(&hb, b); 
	int ret=h_cmp(ha, &hb); 
	h_clear(&hb); 
	return ret;
}

int h_cmpm(hfloat *ha, hfloat* hb) // ��������� �������: ���-��, ��� � cmp(...)
{
	// ���������� �������
	__int32 ss = hb->e - ha->e;
	if(ss<0) return -1;
	if(ss>0) return 1;
	// ���� ������� �����, ���������� ��������
	for(unsigned int i=0;i<getmsize();i++)
	{
		if(ha->m[i] > hb->m[i]) return -1;
		if(ha->m[i] < hb->m[i]) return 1;
	}
	// �������� �����, ������� ����� => ����� �����
	return 0;
}
