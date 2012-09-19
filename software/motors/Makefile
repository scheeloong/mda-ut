CC=g++
FLAGS=-Wall -lm
OBJ=motors.o main.o
SIMDIR=../sim
INC=motors.h $(SIMDIR)/physical_model.h
EXE=motors
A=libmotors.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ -c $(FLAGS)

all: $(EXE) $(A)

$(A): $(EXE) $(INC)
	rm -f $(A)
	ar -cqv $@ $(OBJ)

$(EXE): $(OBJ) $(INC)
	$(CC) $(OBJ) -o $@ $(FLAGS)

clean:
	rm -rf $(OBJ) $(EXE) $(A) *~

.PHONY: all clean
