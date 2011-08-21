// This file defines the parameters for the motor controller

// # of bits for used for PWM frequency
// frequency = 50 MHz / 2^FREQ_NEG_POW
// ie:
//   10 -> 49 KHz
//   12 -> 12 KHz
//   15 -> 1.5 KHz
//   20 -> 48 Hz
//   23 -> 6 Hz
`define FREQ_NEG_POW 12

// Maximum duty cycle (as a fraction)
`define MAX_DUTY_CYCLE (4/5)

// # of dead time cycles to ensure H-bridge does not short
// 500 cycles = 10 microseconds
`define DEAD_TIME 9'd500

// 10 bits to store duty cycle data, for 0.1% resolutio
// (1/2^10 ~ 0.1%)
`define DUTY_CYCLE_SIZE 10
