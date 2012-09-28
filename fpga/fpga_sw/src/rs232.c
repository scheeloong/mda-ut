// Implementation for IMU interface to serial UART

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "sys/alt_irq.h"
#include "system.h"

#include "utils.h"
#include "rs232.h"

#define RS232_CONTROL_OFFSET 1
#define RS232_READ_INTERRUPT 1

#define BUF_LEN 250
#define WRITE_ATTEMPTS 16

// Forward function declarations
static void read_interrupt(void *context, alt_u32 id);

unsigned char checksum_byte(char* command);
unsigned short checksum_short(char* command);

int write_cmd(char *); // Write command (checksum will be added)
int write_str(char *); // Write string (checksum already included)

enum CHECKSUM_MODE {NO_CHECKSUM, BYTE_CHECKSUM, SHORT_CHECKSUM} checksum_mode = BYTE_CHECKSUM;

// Enable read interrupts for RS232
void rs232_init()
{
  IOWR(RS232_0_BASE, RS232_CONTROL_OFFSET, RS232_READ_INTERRUPT);

  // Register read interrupt handler
  alt_ic_isr_register(RS232_0_IRQ_INTERRUPT_CONTROLLER_ID, RS232_0_IRQ, (void *)read_interrupt, 0, 0);
}

void rs232_shell()
{
  puts("Direct output to IMU until you type 'quit'");

  char buf[BUF_LEN+1];

  while (1) {
    alt_getline(buf, BUF_LEN);
    if (!strcmp("quit\n", buf)) {
      break;
    }

    // write to IMU
    buf[strlen(buf)-1] = '\0'; // Strip trailing newline
    write_cmd(buf);
  }
}

// A read from the IMU is ready. Handle it by copying the result.
static void read_interrupt(void *context, alt_u32 id)
{
  // TODO: implement
}

unsigned char checksum_byte(char* command)
{
  unsigned char xor = 0;
  while (*command) {
    xor ^= *command++;
  }
  return xor;
}

unsigned short checksum_short(char* command)
{
  unsigned short crc = 0;
  while (*command) {
    crc = (crc >> 8) | (crc << 8);
    crc ^= *command++;
    crc ^= (crc & 0xff) >> 4;
    crc ^= (crc << 8) << 4;
    crc ^= ((crc & 0xff) << 4) << 1;
  }

  return crc;
}

int write_cmd(char *cmd)
{
  char buf[BUF_LEN+6];

  // Add extra characters and checksum
  if (checksum_mode == NO_CHECKSUM) {
    sprintf(buf, "$%s*", cmd);
  } else if (checksum_mode == BYTE_CHECKSUM) {
    sprintf(buf, "$%s*%02hhx", cmd, checksum_byte(cmd));
  } else {
    sprintf(buf, "$%s*%04hx", cmd, checksum_short(cmd));
  }

  return write_str(buf);
}

int write_str(char *str)
{
  int len = strlen(str);
  int i;
  for (i = 0; i < WRITE_ATTEMPTS; i++) {
    int write_avail = IORD(RS232_0_BASE, RS232_CONTROL_OFFSET) >> 16;
    if (write_avail >= len) {
      break;
    }
  }
  if (i == WRITE_ATTEMPTS) {
    printf("Failed to transmit command string %s\n", str);
    return -1;
  }

  while(*str) {
    IOWR(RS232_0_BASE, 0, *str++);
  }

  return 0;
}
