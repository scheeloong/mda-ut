// Header file for interfacing with the IMU using serial UART

#ifndef _MDA_RS232_H
#define _MDA_RS232_H

void rs232_init();
void rs232_shell();

int get_imu_yaw();
int get_imu_pitch();
int get_imu_roll();

#endif
