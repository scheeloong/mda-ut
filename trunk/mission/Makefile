CC=g++
FLAGS=-Wall -lm
LIBO= mission.o main.o
INC= mission.h
OBJ=mission

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ -c $(FLAGS)

all: $(OBJ)

$(OBJ): $(LIBO)
	$(CC) $(LIBO) -o $@ $(FLAGS)

clean:
	rm -rf $(LIBO) $(OBJ)

.PHONY: all clean
