ifndef NIOSCC
	NIOSCC = nios2-elf-gcc
	FLAG = -D NIOSCC
endif

all:
	$(NIOSCC) -O3 $(FLAG) *.c

clean:
	rm -f a.out
