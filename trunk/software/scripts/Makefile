# FPAG UI makefile

CC=gcc
CFLAGS = -Wall -lm
INC=scripts.h
UIDIR=../../fpga/ui/
UIA=libui.a
AFLAGS=-L$(UIDIR) $(UIDIR)$(UIA)

all: move_up_down

move_up_down.o: $(INC) move_up_down.c
	$(CC) -c move_up_down.c $(CFLAGS)

move_up_down: move_up_down.o
	$(CC) $@.o $(AFLAGS) $(CFLAGS) -o $@

clean:
	rm -f *.o log.txt move_up_down

.PHONY: all clean
