CC=g++
FLAGS=-c -Wall
LIBO= mission.o main.o
INC= mission.h
OBJ=mission

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ $(FLAGS)

all: $(OBJ)

$(OBJ): $(LIBO)
	$(CC) $(LIBO) -o $@ -Wall

clean:
	rm -rf $(LIBO) $(OBJ)

.PHONY: all clean
