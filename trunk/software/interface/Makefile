CC=g++
CVFLAGS=`pkg-config --cflags --libs opencv` -lvision -L../vision/
GLFLAGS=-lm -lGL -lglut -lGLU -lsim -L../sim
CFLAGS=-O3 -Wall -I ../sim -I ../vision/lib -pthread -lncurses
ALLFLAGS=$(CVFLAGS) $(GLFLAGS) $(CFLAGS) -lui -L../../fpga/ui/

INC=*.h
SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
LIBVISION=../vision/libvision.a
LIBSIM=../sim/libsim.a
LIBUI=../../fpga/ui/libui.a
LIB=$(LIBVISION) $(LIBSIM) $(LIBUI)

AQUATUX=aquatux

all: $(AQUATUX)

$(AQUATUX): $(OBJ) $(LIB)
	$(CC) -o $@ $(OBJ) $(LIB) $(ALLFLAGS)

%.o: %.cpp $(INC)
	$(CC) -c $< -o $@ $(CVFLAGS) $(CFLAGS)

$(LIBVISION):
	make -C ../vision `basename $@`

$(LIBSIM):
	make -C ../sim `basename $@`

$(LIBUI):
	make -C ../../fpga/ui `basename $@`

clean:
	rm -f $(OBJ)

.PHONY: all clean
