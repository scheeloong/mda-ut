ifndef NIOSCC
	NIOSCC = nios2-elf-gcc
	FLAG = -D NIOSCC -o mda.elf
endif

all:
	$(NIOSCC) -O3 $(FLAG) *.c *.s

clean:
	rm -f mda.elf
