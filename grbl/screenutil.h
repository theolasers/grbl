#ifndef SCREENUTIL_H_
#define SCREENUTIL_H_

#include <avr/pgmspace.h>
#include <Print.h>

char *ftostr4(const float x, char *conv);
char *ftostr31ns(const float x, char *conv);
char *ftostr31(const float x, char *conv);

class PrintExt : public Print {
public:
	PrintExt() {}
	virtual ~PrintExt() {}

	void printPgmString(const char *s);
	void print_unsigned_int8(uint8_t n, uint8_t base, uint8_t digits);
};

#endif /* SCREENUTIL_H_ */
