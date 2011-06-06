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

int main(){
	alt_u32 sw;
	alt_u32 hex03;

	IOWR_ALTERA_AVALON_PIO_DATA(HEX47_BASE, str_to_seven_seg("mda "));
	IOWR_ALTERA_AVALON_PIO_DATA(HEX03_BASE, str_to_seven_seg("uoft"));


	while(1){
		sw = IORD_ALTERA_AVALON_PIO_DATA(SW_BASE);
		IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, sw);
	}

	return 0;
}
