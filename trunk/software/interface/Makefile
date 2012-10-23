CC=g++ -O3 -Wall -I ../sim

INC=*.h
SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))

all: $(OBJ)

%.o: %.cpp $(INC)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ)

.PHONY: all clean
