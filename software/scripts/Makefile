CC=gcc
CFLAGS = -Wall -lm

INC=scripts.h
UIDIR=../../fpga/ui/
UIA=libui.a
AFLAGS=-L$(UIDIR) $(UIDIR)$(UIA)

EXES=move_up_down.out

all: $(EXES)

%.out: %.c $(INC) $(UIDIR)$(UIA)
	$(CC) $< $(AFLAGS) $(CFLAGS) -o $@

clean:
	rm -f log.txt $(EXES)

.PHONY: all clean
