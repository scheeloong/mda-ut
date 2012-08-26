CC=g++
FLAGS=-Wall -lm
LIBO=motors.o main.o
INC=motors.h
OBJ=motors
A=libmotors.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ -c $(FLAGS)

all: $(OBJ) $(A)

$(A): $(OBJ)
	ar -cqv $@ $(LIBO)

$(OBJ): $(LIBO)
	$(CC) $(LIBO) -o $@ $(FLAGS)

clean:
	rm -rf $(LIBO) $(OBJ) $(A) *~

.PHONY: all clean
