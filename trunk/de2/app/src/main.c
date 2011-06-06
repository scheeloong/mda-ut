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

int main(){
	alt_u32 sw;
	while(1){
		sw = IORD_ALTERA_AVALON_PIO_DATA(SW_BASE);
		IOWR_ALTERA_AVALON_PIO_DATA(LEDR_BASE, sw);
	}

	return 0;
}
