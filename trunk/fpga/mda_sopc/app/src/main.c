#include "main.h"
#include "system.h"
#include "sys/alt_stdio.h"

int main() {
	while (1) {
		alt_putstr(MSG);
		while (alt_getchar() != '\n')
			;
	}
	return 0;
}
