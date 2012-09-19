CC=gcc
CFLAGS = -Wall -lm

INC=scripts.h
UIDIR=../../fpga/ui/
UIA=libui.a
AFLAGS=-L$(UIDIR) $(UIDIR)$(UIA)

SRCS:=$(wildcard *.c)
EXES:=$(patsubst %.c,%.out,$(SRCS))

all: $(EXES)

%.out: %.c $(INC) $(UIDIR)$(UIA)
	$(CC) $< $(AFLAGS) $(CFLAGS) -o $@

$(UIDIR)$(UIA):
	make -C $(UIDIR)

clean:
	rm -f log.txt $(EXES)

.PHONY: all clean
