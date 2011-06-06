#include "alt_types.h"
#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "sys/alt_irq.h"
#include "altera_avalon_pio_regs.h"

int main(){

	printf("Hello world\n");
	IOWR_ALTERA_AVALON_PIO_DATA(LEDG_BASE, 0xbe);

	while(1){

	}

	return 0;
}
