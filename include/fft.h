#if !defined(__LUFTWAFFE_ACE_HAINZ_ULRICH_RUDEL)
#define __LUFTWAFFE_ACE_HAINZ_ULRICH_RUDEL

#define twoPI 2*3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865
const unsigned char PARTSIZE=8;

extern unsigned int COMPLEX_COUNT;

class complex {
public:
	double re,im;
	complex() {im=0; re=0; };
	complex(double r, double i) {re=r; im=i;}
	const complex operator=(const complex &c) { re = c.re; im = c.im; return *this;}
	const complex operator+(const complex &c) const { return complex(re+c.re, im+c.im); }
	const complex operator-(const complex &c) const { return complex(re-c.re, im-c.im); }
	const complex operator*(const complex &c) const { return complex(re*c.re - im*c.im, re*c.im + im*c.re); }
};

void __stdcall complex_add(complex* a, complex* b, complex* r);

void __stdcall complex_sub(complex* a, complex* b, complex* r);

void __stdcall complex_mul(complex* a, complex* b, complex* r);

void __stdcall complex_copy(complex* tgt, complex* src);

// МЕДЛЕННЫЕ ПРЕОБРАЗОВАНИЯ ФУРЬЕ
// прямое...
void forward_ft(complex* s, complex* ft, unsigned int size);

// обратное
void inverse_ft(complex* ft, complex* s, unsigned int size);

// БЫСТРОЕ ПРЕОБРАЗОВАНИЕ ФУРЬЕ
// инициализация fft : построение таблицы корней
void fft_init(unsigned int sampsize);
void fft_deinit();

// прямое...
void forward_fft(double *s, complex *ft, unsigned int sampsize);

// обратное
void inverse_fft(complex *ft, double *s, unsigned int sampsize);

#endif // __LUFTWAFFE_ACE_HAINZ_ULRICH_RUDEL