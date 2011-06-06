/*
 * main.c
 *
 *  Created on: 2011-06-06
 *      Author: mark
 */

#include "alt_types.h"
#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"

#include "seven_seg.h"

/*
 * This is just a simple program that tests the capabilities of the NIOS2 on the DE2 board
 * I got most of this info from
 * /opt/altera/latest/nios2eds/examples/software/board_diag/
 */

volatile int edge_capture;


static void handle_key_interrupts(void * context){
	volatile int *edge_capture_ptr = (volatile int *)context;
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0);

	IOWR_ALTERA_AVALON_PIO_DATA(LEDG_BASE, *edge_capture_ptr);

	// Read to delay stuff
	IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
}

int main(){
	alt_u32 sw;

	IOWR_ALTERA_AVALON_PIO_DATA(HEX47_BASE, str_to_seven_seg("mda "));
	IOWR_ALTERA_AVALON_PIO_DATA(HEX03_BASE, str_to_seven_seg("uoft"));

	FILE *lcd;
	lcd = fopen(LCD_NAME, "w");
#define ESC 27
#define CLEAR_LCD_STRING "[2J"
	if(lcd != NULL){
		// It actually goes in here, but doesn't do anything
		// You can check by enabling this command
		// IOWR_ALTERA_AVALON_PIO_DATA(LEDG_BASE, 0xff);
		// I don't know why this doesn't work, maybe it is a different LCD on the DE2 board
		// NOt really worth my time
		fprintf(lcd, "\nTesting the LCD\n");
		fprintf(lcd, "By Mark Harfouche\n");


		// This command clears the LCD
		// fprintf(lcd, "%c%s", ESC, CLEAR_LCD_STRING);
		fclose(lcd);
	}


	// set up the push_bottun interrupts
	
	void * edge_capture_ptr = (void *) &edge_capture;
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xf);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0x0);
	alt_ic_isr_register(KEY_IRQ_INTERRUPT_CONTROLLER_ID, KEY_IRQ, handle_key_interrupts, edge_capture_ptr, 0x0);

	while(1){
		sw = IORD_ALTERA_AVALON_PIO_DATA(SW_BASE);
		IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, sw);
	}

	return 0;
}
