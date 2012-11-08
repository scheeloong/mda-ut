// Header file for interfacing with the IMU using serial UART

#ifndef _MDA_RS232_H
#define _MDA_RS232_H

#include <stdbool.h>

#include "controller.h"

void rs232_init();
void rs232_shell();

bool imu_ready();
void get_imu_orientation(struct orientation *);

#endif
