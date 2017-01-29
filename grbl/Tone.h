#ifndef TONE_H_
#define TONE_H_

#include "grbl.h"

#ifdef __cplusplus
extern "C" {
#endif

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration);
void noTone(uint8_t _pin);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
