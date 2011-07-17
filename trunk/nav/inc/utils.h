// Function prototypes implemented in utils.c

#ifndef _MDA_UTILS_H
#define _MDA_UTILS_H

#define STR_LEN 30
#include "accelerometer_adxl345_spi.h"

void alt_getline(char *, int);
int read_hex(char *);
void get_accel(struct t_accel_data *);

#endif
