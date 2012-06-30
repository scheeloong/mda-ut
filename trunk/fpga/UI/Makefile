# FPAG UI makefile

CC=gcc -Wall
INC=utils.h
OBJ=utils.o
EXE=UI

all: fpga_ui

.c.o: $(INC)
	$(CC) -c $< -o $@

fpga_ui: utils.o main.o
	$(CC) main.o $(OBJ) -o $(EXE)

clean:
	rm -f UI *.o *~
