# FPGA UI makefile

CC=gcc
CFLAGS = -Wall -lm
INC=utils.h
OBJ=utils.o
EXE=ui
A=libui.a

all: $(EXE) $(A)

.c.o: $(INC)
	$(CC) -c $< $(CFLAGS) -o $@

$(EXE): utils.o main.o
	$(CC) main.o $(OBJ) $(CFLAGS) -o $(EXE)

$(A): $(EXE) $(INC)
	rm -f $(A)
	ar -cqv $@ $(OBJ)

clean:
	rm -f $(EXE) $(A) $(OBJ) *~
