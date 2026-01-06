#ifndef UTIL_H
#define UTIL_H

#include "gba.h"

void memcpy(char *dst, const char *src, unsigned int size);
void memset(char *dst, char val, unsigned int size);
void fmt_u32(u32 value, char* buf);

#endif /* UTIL_H */
