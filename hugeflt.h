#if !defined(__ERRARE_HUMANUM_EST)
#define __ERRARE_HUMANUM_EST

#include <altgmp.h>

// CPP-интерфейс
class CHugeFloat {
public:
	hfloat m_hfloat;
	CHugeFloat() { h_init(&m_hfloat); }
	CHugeFloat(double d) { h_init(&m_hfloat); h_copy_d(&m_hfloat, d); }
	CHugeFloat(CHugeFloat& ha) { h_init(&m_hfloat); h_copy(&m_hfloat, &ha.m_hfloat); }
	CHugeFloat(hfloat& ha) { h_init(&m_hfloat); h_copy(&m_hfloat, &ha); }
	~CHugeFloat() { h_clear(&m_hfloat); }
	
	CHugeFloat operator=(CHugeFloat& ha) { h_copy(&m_hfloat, &ha.m_hfloat); return CHugeFloat(*this); }
	CHugeFloat operator=(double a) { h_copy_d(&m_hfloat, a); return CHugeFloat(*this); }
	CHugeFloat operator=(hfloat& ha) { h_copy(&m_hfloat, &ha); return CHugeFloat(*this); }
	CHugeFloat operator=(char* a) 
	{ 
		if(a[0]=='0')
			switch(a[1]) {
			case 'x':
			case 'X': h_atoh(&m_hfloat, a+2, 16); return CHugeFloat(*this);
			case '%': h_atoh(&m_hfloat, a+2, 2); return CHugeFloat(*this);
			case '.': h_atoh(&m_hfloat, a, 10); return CHugeFloat(*this);
			default: if((a[1]>='0')&&(a[1]<='7')) h_atoh(&m_hfloat, a+1, 8); return CHugeFloat(*this);
		}
		h_atoh(&m_hfloat, a, 10); 
		return CHugeFloat(*this);
	}

	CHugeFloat operator-() { hfloat hr; h_init(&hr); h_copy(&hr, &m_hfloat); h_neg(&hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }

	CHugeFloat operator+(CHugeFloat& ha) { hfloat hr; h_init(&hr); h_add(&m_hfloat, &ha.m_hfloat, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	CHugeFloat operator+(double a) { hfloat hr, ha; h_init(&hr); h_init(&ha); h_copy_d(&ha, a); h_add(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); h_clear(&ha); return ret; }
	CHugeFloat operator+(hfloat& ha) { hfloat hr; h_init(&hr); h_add(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	void operator+=(CHugeFloat& ha) { h_add(&m_hfloat, &ha.m_hfloat, &m_hfloat); }
	void operator+=(double a) { hfloat ha; h_init(&ha); h_copy_d(&ha, a); h_add(&m_hfloat, &ha, &m_hfloat); h_clear(&ha); }
	void operator+=(hfloat& ha) {h_add(&m_hfloat, &ha, &m_hfloat); }

	CHugeFloat operator-(CHugeFloat& ha) { hfloat hr; h_init(&hr); h_sub(&m_hfloat, &ha.m_hfloat, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	CHugeFloat operator-(double a) { hfloat hr, ha; h_init(&hr); h_init(&ha); h_copy_d(&ha, a); h_sub(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); h_clear(&ha); return ret; }
	CHugeFloat operator-(hfloat& ha) { hfloat hr; h_init(&hr); h_sub(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	void operator-=(CHugeFloat& ha) { h_sub(&m_hfloat, &ha.m_hfloat, &m_hfloat); }
	void operator-=(double a) { hfloat ha; h_init(&ha); h_copy_d(&ha, a); h_sub(&m_hfloat, &ha, &m_hfloat); h_clear(&ha);}
	void operator-=(hfloat& ha) {h_sub(&m_hfloat, &ha, &m_hfloat); }

	CHugeFloat operator*(CHugeFloat& ha) { hfloat hr; h_init(&hr); h_mul(&m_hfloat, &ha.m_hfloat, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	CHugeFloat operator*(double a) { hfloat hr, ha; h_init(&hr); h_init(&ha); h_copy_d(&ha, a); h_mul(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); h_clear(&ha); return ret; }
	CHugeFloat operator*(hfloat& ha) { hfloat hr; h_init(&hr); h_mul(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	void operator*=(CHugeFloat& ha) { h_mul(&m_hfloat, &ha.m_hfloat, &m_hfloat); }
	void operator*=(double a) { hfloat ha; h_init(&ha); h_copy_d(&ha, a); h_mul(&m_hfloat, &ha, &m_hfloat); h_clear(&ha); }
	void operator*=(hfloat& ha) {h_mul(&m_hfloat, &ha, &m_hfloat); }

	CHugeFloat operator/(CHugeFloat& ha) { hfloat hr; h_init(&hr); h_div(&m_hfloat, &ha.m_hfloat, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	CHugeFloat operator/(double a) { hfloat hr, ha; h_init(&hr); h_init(&ha); h_copy_d(&ha, a); h_div(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); h_clear(&ha); return ret; }
	CHugeFloat operator/(hfloat& ha) { hfloat hr; h_init(&hr); h_div(&m_hfloat, &ha, &hr); CHugeFloat ret(hr); h_clear(&hr); return ret; }
	void operator/=(CHugeFloat& ha) { h_div(&m_hfloat, &ha.m_hfloat, &m_hfloat); }
	void operator/=(double a) { hfloat ha; h_init(&ha); h_copy_d(&ha, a); h_div(&m_hfloat, &ha, &m_hfloat); h_clear(&ha); }
	void operator/=(hfloat& ha) {h_div(&m_hfloat, &ha, &m_hfloat); }

	operator double() { return h_getdouble(&m_hfloat); }

	bool operator>(hfloat &other) { return h_cmp(&m_hfloat,&other)==-1?true:false; }
	bool operator<(hfloat &other) { return h_cmp(&m_hfloat,&other)==1?true:false; }
	bool operator>=(hfloat &other) { return h_cmp(&m_hfloat,&other)<=0?true:false; }
	bool operator<=(hfloat &other) { return h_cmp(&m_hfloat,&other)>=0?true:false; }
	bool operator==(hfloat &other) { return h_cmp(&m_hfloat,&other)==0?true:false; }
	bool operator!=(hfloat &other) { return h_cmp(&m_hfloat,&other)!=0?true:false; }
	
	bool operator>(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)==-1?true:false; h_clear(&hb); return ret;}
	bool operator<(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)==1?true:false; h_clear(&hb); return ret;}
	bool operator>=(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)<=0?true:false; h_clear(&hb); return ret;}
	bool operator<=(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)>=0?true:false; h_clear(&hb); return ret;}
	bool operator==(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)==0?true:false; h_clear(&hb); return ret;}
	bool operator!=(double other) { hfloat hb; h_init(&hb); h_copy_d(&hb, other); bool ret=h_cmp(&m_hfloat,&hb)!=0?true:false; h_clear(&hb); return ret;}

	bool iszero() { return h_iszero(&m_hfloat); }

	void htoa(char* buf, unsigned int ccmax, int radix) { h_htoa(&m_hfloat, buf, ccmax, radix); }
	unsigned int atoh(char* buf, int radix) { return h_atoh(&m_hfloat, buf, radix); }
	void clfrac() { h_clfrac(&m_hfloat); }
	void clrat()  { h_clrat(&m_hfloat); }
};

#endif // __ERRARE_HUMANUM_EST