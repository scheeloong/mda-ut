CC=g++
FLAGS=-Wall -lm
LIBO=mission.o main.o
INC=mission.h
OBJ=mission
A=libmission.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ -c $(FLAGS)

all: $(OBJ) $(A)

$(A): $(OBJ)
	ar -cqv $@ $(LIBO)

$(OBJ): $(LIBO)
	$(CC) $(LIBO) -o $@ $(FLAGS)

clean:
	rm -rf $(LIBO) $(OBJ) $(A)

.PHONY: all clean
