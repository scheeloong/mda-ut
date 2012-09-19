CC=gcc
CFLAGS = -Wall -lm

INC=scripts.h
UIDIR=../../fpga/ui/
UIA=libui.a
AFLAGS=-L$(UIDIR) $(UIDIR)$(UIA)

EXES=move_up_down

all: $(EXES)

.c.o: $(INC)
	$(CC) -c $< $(CFLAGS)

$(EXES): *.o
	$(CC) $@.o $(AFLAGS) $(CFLAGS) -o $@

clean:
	rm -f *.o log.txt $(EXES)

.PHONY: all clean
