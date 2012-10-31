// altgmp.h: declaring hfloat struct, hfloat functions and interface for the CHugeFloat class
//
//////////////////////////////////////////////////////////////////////

#if !defined(__NICOLE_KIDMAN_IS_A_PRETTY_WOMAN)
#define __NICOLE_KIDMAN_IS_A_PRETTY_WOMAN

#define _MSIZE 10

extern unsigned __int32 MSIZE;
extern unsigned __int32 LMSIZE;

typedef struct {
	__int8 s; // знак
	__int32 e; // пор€док
	unsigned __int32 *m; // мантисса
} hfloat;

extern "C"  void jumo_init(unsigned __int32 s);
extern "C"  void jumo_deinit();

extern "C"  inline const unsigned __int32 getmsize();

extern "C"  void h_init(hfloat* ha);
extern "C"  void h_clear(hfloat* ha);

extern "C"  void h_copy_d(hfloat* dest, double src);
extern "C"  void h_copy(hfloat* dest, hfloat* src);
extern "C"  double h_getdouble(hfloat* src);

extern "C"  void h_neg(hfloat* ha); // unary minus

extern "C"  void h_add(hfloat* ha, hfloat* hb, hfloat* hr);
extern "C"  void h_add_d(hfloat* ha, double b, hfloat* hr);

extern "C"  void h_sub(hfloat* ha, hfloat* hb, hfloat* hr);
extern "C"  void h_sub_d(hfloat* ha, double b, hfloat* hr);

extern "C"  void h_mul(hfloat* ha, hfloat* hb, hfloat* hr);
extern "C"  void h_mul_d(hfloat* ha, double b, hfloat* hr);

extern "C"  void h_div(hfloat* ha, hfloat* hb, hfloat* hr);
extern "C"  void h_div_d(hfloat* ha, double b, hfloat* hr);


extern "C"  void h_htoa(hfloat* ha, char* buf, unsigned int ccmax, int radix);
extern "C"  unsigned int h_atoh(hfloat* ha, char* buf, int radix);
extern "C"  void h_clfrac(hfloat* ha);
extern "C"  void h_clrat(hfloat* ha);
extern "C"  void h_norm(hfloat* ha);

extern "C"  bool h_iszero(hfloat* ha);
extern "C"  int h_cmp(hfloat *ha, hfloat* hb); // сравнение чисел: -1: а>b, 0: a==b, 1: a<b
extern "C"  int h_cmp_d(hfloat *ha, double b); // сравнение чисел: -1: а>b, 0: a==b, 1: a<b
extern "C"  int h_cmpm(hfloat *ha, hfloat* hb); // сравнение модулей: так-же, как в cmp(...)

extern "C"  void h_mneg(hfloat* ha); // инвертирует мантиссу, прибавл€ет 1 (делает отрицательной в ƒ.ќ. )
extern "C"  void h_mshl(hfloat* ha, unsigned int count); // сдвиг мантиссы влево
extern "C"  void h_mshr(hfloat* ha, unsigned int count); // сдвиг мантиссы вправо

#pragma message (" ")
#pragma message ("...you can't see my eyes")
#pragma message ("and they don't see yours")
#pragma message ("hear me when i say i don't mind at all")
#pragma message ("it's the rain that i hear coming")
#pragma message ("not a stranger or a ghost")
#pragma message ("the quiet of a storm approaching")
#pragma message ("that i fear the most")
#pragma message ("it's the pain that i hear coming")
#pragma message ("the slightest crystal tear")
#pragma message ("drops to the ground in silence")
#pragma message ("when my love is near...")
#pragma message (" ")


#endif // !defined(__NICOLE_KIDMAN_IS_A_PRETTY_WOMAN)
