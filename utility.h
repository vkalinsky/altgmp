#if !defined(__TERTIUM_NON_DAIUR)
#define __TERTIUM_NON_DAIUR

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

template<class _T> _T max(_T a,_T b) {return a>b?a:b;}
template<class _T> _T min(_T a,_T b) {return a<b?a:b;}

// служебные naked-функции
// параметры передаются через регистры

// сдвиг мантиссы влево
// в ebx - адрес мантиссы
void _mshl();

// сдвиг мантиссы вправо
// в ebx - адрес мантиссы
void _mshr();

// _madd : сложение массивов limb-ов в дополнительном обратном коде
// ebx - первое слагаемое
// esi - второе слагаемое
// edi - результат
void _madd();

// переводит число в десятичный вид
// eax - число
// esi - адрес строкового буфера для числа
// ch  - писать ли незначащие нули в начале: 0 - не писать, >0 - писать
// на выходе в ebx - количество символов, записанных в буфер
void ascii4();

#endif // __TERTIUM_NON_DAIUR