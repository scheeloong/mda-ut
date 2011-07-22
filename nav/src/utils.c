/*
 * utils.c
 *
 * Useful utility functions
 *
 * Author: victor
 */

#include "alt_types.h"
#include "io.h"
#include "system.h"
#include "sys/alt_stdio.h"

#include "settings.h"
#include "utils.h"

void alt_getline(char *st, int len)
{
  while (len--) {
    char c = (char)alt_getchar();
    if (INTERACTIVE) {
      alt_putchar(c);
    }
    *st++ = c;
    if (c == '\n')
      break;
  }
  *st = '\0';
}

int read_hex(char *st)
{
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

void get_accel(struct t_accel_data *accel_data)
{
  IOWR(SELECT_I2C_CLK_BASE, 0, 0x00);

  // configure accelerometer as +-2g and start measure
  if (!ADXL345_SPI_Init(GSENSOR_SPI_BASE)) {
    // could not configure accelerometer
    return;
  }

  while (1) {
    if (ADXL345_SPI_IsDataReady(GSENSOR_SPI_BASE) && ADXL345_SPI_XYZ_Read(GSENSOR_SPI_BASE, accel_data)) {
      break;
    }
  }

  return;
}
