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
#include "altera_avalon_timer_regs.h"

#include "seven_seg.h"

/*
 * This is just a simple program that tests the capabilities of the NIOS2 on the DE2 board
 * I got most of this info from
 * /opt/altera/latest/nios2eds/examples/software/board_diag/
 */

volatile int edge_capture;
volatile int timer_1=0;


static void handle_key_interrupts(void * context){
	volatile int *edge_capture_ptr = (volatile int *)context;
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0);

	IOWR_ALTERA_AVALON_PIO_DATA(LEDG_BASE, *edge_capture_ptr);

	// Read to delay stuff
	IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
}

static void handle_timer_1_interrupts(void *context){
	volatile int *timer = (volatile int *)context;
	(*timer) ++;

	IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, *timer);

	// write 0 to ackwoledge the interrupt
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0x0);
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
		// 16 chars   1234567890123456
		fprintf(lcd, "MDA test code By\n");
		fprintf(lcd, " Mark Harfouche \n");


		// This command clears the LCD
		// fprintf(lcd, "%c%s", ESC, CLEAR_LCD_STRING);
		fclose(lcd);
	}
	
	// Setup TIMER_1 (I think timer_0 is used for the system clock, so I'll leave that on its own)
	// The timer data gets sent in chuncks of 16 bits
	
	timer_1 = 0;
	// stop it
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, ALTERA_AVALON_TIMER_CONTROL_STOP_MSK);
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_1_BASE, 0x0);

	// write the period
	IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_1_BASE, 50000000);
	IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_1_BASE, 50000000>>16);

	alt_ic_isr_register(TIMER_1_IRQ_INTERRUPT_CONTROLLER_ID, TIMER_1_IRQ, handle_timer_1_interrupts, &timer_1, 0x0);
	// Start it 
	IOWR_ALTERA_AVALON_TIMER_CONTROL(TIMER_1_BASE, 
			ALTERA_AVALON_TIMER_CONTROL_ITO_MSK | 
			ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
			ALTERA_AVALON_TIMER_CONTROL_START_MSK);

	


	// set up the push_bottun interrupts
	
	void * edge_capture_ptr = (void *) &edge_capture;
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY_BASE, 0xf);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE, 0x0);
	alt_ic_isr_register(KEY_IRQ_INTERRUPT_CONTROLLER_ID, KEY_IRQ, handle_key_interrupts, edge_capture_ptr, 0x0);

	while(1);
	// This is a simple example of how to read from the keys
	//while(1){
		//sw = IORD_ALTERA_AVALON_PIO_DATA(SW_BASE);
		//IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, sw);
	//}

	return 0;
}
