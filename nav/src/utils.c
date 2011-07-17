/*
 * utils.c
 *
 * Useful utility functions
 *
 * Author: victor
 */

#include "alt_types.h"
#include "sys/alt_stdio.h"
#include "io.h"
#include "system.h"

#include "accelerometer_adxl345_spi.h"

void alt_getline(char *st, int len)
{
  while (len--) {
    char c = (char)alt_getchar();
	alt_putchar(c);
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

void print_accel(void)
{
  // accelerometer data in each direction
  struct t_accel_data accel_data;

  IOWR(SELECT_I2C_CLK_BASE, 0, 0x00);
    
  // configure accelerometer as +-2g and start measure
  if (!ADXL345_SPI_Init(GSENSOR_SPI_BASE)) {
    // could not configure accelerometer
    return;
  }
    
  while (1) {
    if (ADXL345_SPI_IsDataReady(GSENSOR_SPI_BASE) && ADXL345_SPI_XYZ_Read(GSENSOR_SPI_BASE, accel_data)) {
      // multiply each struct member by 4 to get the acceleration in mg and print in hexadecimal
      alt_printf("X=%x mg, Y=%x mg, Z=%x mg\n", accel_data.x * 4, accel_data.y * 4, accel_data.z * 4);
      break;
    }
  }

  return;
}
