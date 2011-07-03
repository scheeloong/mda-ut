/*
 * utils.c
 *
 * Useful utility functions
 *
 * Author: victor
 */

#include "sys/alt_stdio.h"

void alt_getline(char *st, int len) {
  while (len--) {
    char c = (char)alt_getchar();
	alt_putchar(c);
    *st++ = c;
    if (c == '\n')
      break;
  }
  *st = '\0';
}

int read_hex (char *st) {
  int i = 0;
  while (*st && *st != '\n') {
    i <<= 4;
    if (*st >= '0' && *st <= '9')
      i |= (int)(*st - '0');
    else if (*st >= 'a' && *st <= 'f')
      i |= (int)(*st - 'a' + 10);
    st++;
  }
  return i;
}
