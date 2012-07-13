# FPAG UI makefile

CC=gcc
CFLAGS = -Wall -lm
INC=utils.h
OBJ=utils.o
EXE=ui

all: fpga_ui

.c.o: $(INC)
	$(CC) -c $< $(CFLAGS) -o $@

fpga_ui: utils.o main.o
	$(CC) main.o $(OBJ) $(CFLAGS) -o $(EXE)

clean:
	rm -f $(EXE) *.o *~
