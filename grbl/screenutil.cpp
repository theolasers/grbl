#include <stdlib.h>
#include "screenutil.h"

char *ftostr4(const float x, char *conv)
{
  unsigned int xx=x*10;
  conv[0]=(xx/10000)%10+'0';
  conv[1]=(xx/1000)%10+'0';
  conv[2]=(xx/100)%10+'0';
  conv[3]=(xx/10)%10+'0';
  conv[4]=0;
  return conv;
}

char *ftostr31ns(const float x, char *conv)
{
  int xx=x*10;
  conv[0]=(xx/1000)%10+'0';
  conv[1]=(xx/100)%10+'0';
  conv[2]=(xx/10)%10+'0';
  conv[3]='.';
  conv[4]=(xx)%10+'0';
  conv[5]=0;
  return conv;
}

char *ftostr31(const float x, char *conv)
{
  int xx=x*10;
  conv[0]=(xx>=0)?'+':'-';
  xx=abs(xx);
  conv[1]=(xx/1000)%10+'0';
  conv[2]=(xx/100)%10+'0';
  conv[3]=(xx/10)%10+'0';
  conv[4]='.';
  conv[5]=(xx)%10+'0';
  conv[6]=0;
  return conv;
}

// Print a string stored in PGM-memory
void PrintExt::printPgmString(const char *s) {
  char c;
  while ((c = pgm_read_byte_near(s++)))
    write(c);
}

void PrintExt::print_unsigned_int8(uint8_t n, uint8_t base, uint8_t digits) {
  unsigned char buf[digits];
  uint8_t i = 0;

  for (; i < digits; i++) {
      buf[i] = n % base ;
      n /= base;
  }

  for (; i > 0; i--)
	  write('0' + buf[i - 1]);
}
