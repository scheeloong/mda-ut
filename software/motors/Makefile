CC=g++
FLAGS=-Wall -lm
LIBO=motors.o main.o
SIMDIR=../sim
INC=motors.h $(SIMDIR)/physical_model.h
OBJ=motors
A=libmotors.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ -c $(FLAGS)

all: $(OBJ) $(A)

$(A): $(OBJ) $(INC)
	rm -f $(A)
	ar -cqv $@ $(LIBO)

$(OBJ): $(LIBO) $(INC)
	$(CC) $(LIBO) -o $@ $(FLAGS)

clean:
	rm -rf $(LIBO) $(OBJ) $(A) *~

.PHONY: all clean
