
#include "stdafx.h"

uint32 reverse_copy_dword(void *dst, uint32 src)
// копирует 4 байта src->dst с изменением порядка байтов на обратный: src[0]->dst[3]...
// возвращает dst в виде dword
{
  uint8 *s = (uint8 *) &src, *d = (uint8 *) dst;
  d[0] = s[3], d[1] = s[2], d[2] = s[1], d[3] = s[0];
  return *(uint32 *) dst;
}

uint64 C_n_k(int n, int k)
// возвращает число сочетаний из n по k: C = n!/k!*(n-k)! = (k+1)*(k+2)*...*n/1*2*...*(n-k)
// эта функция "загибается" уже при n = 21, k = 2
{
  if (n <= 0 || k <= 0 || n < k) return 0;
  int i;
  uint64 verh = 1, niz = 1;
  for (i = k+1; i <= n; ++i) verh *= i;
  for (i = 2; i <= (n-k); ++i) niz *= i;
  return uint64(verh/niz);
}

uint64 fC_n_k(int n, int k)
// возвращает число сочетаний из n по k: C = n!/k!*(n-k)! = (k+1)*(k+2)*...*n/1*2*...*(n-k)
// вариант с плав. запятой, даёт гораздо больший рабочий диапазон!
{
  if (n <= 0 || k <= 0 || n < k) return 0;
  int i;
  double verh = 1., niz = 1.;
  for (i = k+1; i <= n; ++i) verh *= i;
  for (i = 2; i <= (n-k); ++i) niz *= i;
  return uint64(0.5 + verh/niz);
}

// comment flip-flop trigger: start from //* or /*

//* 
  static int C777;
/*/
  static int D777;
//*/

inline uint32 Randu(uint32 &seed)
{
  static const uint32 RANDUMUL = 1686629717u;
  static const uint32 RANDUADD =  907633385u;
  return (seed = RANDUMUL*seed+RANDUADD);
}

inline int Randu31(uint32 &seed)
{
  // убираем знаковый бит, результат положительный!
  return ( 0x7FFFFFFF & Randu(seed) );
}

inline int Randu16(uint32 &seed)
{
  return ( 0xFFFF & ( Randu(seed) >> 16 ) ); // 0...65535
}

inline int Randu8(uint32 &seed)
{
  return ( 0xFF & ( Randu(seed) >> 24 ) ); // 0...255
}

inline void mem32cpy32(void *dst, void *src, int numdwords)
{
  _asm
  {
    mov   edi, dword ptr dst
    mov   esi, dword ptr src
    mov   ecx, dword ptr numdwords
    cmp   ecx, 0
    jle   end
    cld                 
    rep  movsd
    end:
  }
}

inline void mem32set32(void *dst, int setval, int numdwords)
{
  _asm
  {
    mov   edi, dword ptr dst
    mov   eax, dword ptr setval
    mov   ecx, dword ptr numdwords
    cmp   ecx, 0
    jle   end
    cld                 
    rep  stosd
    end:
  }
}

void SetBit(int &mask, int bitnum, int bitval)
{
  if ( bitnum < 0 || bitnum > 31 ) return;

  int bit = 1 << bitnum;
  int negbit = 0xFFFFFFFF^bit;

  if ( bitval == 0 ) mask = mask & negbit;
  else               mask = mask | bit;
}

