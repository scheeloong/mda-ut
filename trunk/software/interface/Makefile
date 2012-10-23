CC=g++ -O3 -Wall -I ../sim -I ../vision/lib
CVFLAGS=`pkg-config --cflags --libs opencv`

INC=*.h
SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))

all: $(OBJ)

%.o: %.cpp $(INC)
	$(CC) -c $< -o $@ $(CVFLAGS)

clean:
	rm -f $(OBJ)

.PHONY: all clean
