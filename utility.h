#if !defined(__TERTIUM_NON_DAIUR)
#define __TERTIUM_NON_DAIUR

#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

template<class _T> _T max(_T a,_T b) {return a>b?a:b;}
template<class _T> _T min(_T a,_T b) {return a<b?a:b;}

// ��������� naked-�������
// ��������� ���������� ����� ��������

// ����� �������� �����
// � ebx - ����� ��������
void _mshl();

// ����� �������� ������
// � ebx - ����� ��������
void _mshr();

// _madd : �������� �������� limb-�� � �������������� �������� ����
// ebx - ������ ���������
// esi - ������ ���������
// edi - ���������
void _madd();

// ��������� ����� � ���������� ���
// eax - �����
// esi - ����� ���������� ������ ��� �����
// ch  - ������ �� ���������� ���� � ������: 0 - �� ������, >0 - ������
// �� ������ � ebx - ���������� ��������, ���������� � �����
void ascii4();

#endif // __TERTIUM_NON_DAIUR