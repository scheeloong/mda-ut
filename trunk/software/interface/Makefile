CVFLAGS=`pkg-config --cflags --libs opencv` -lvision -L../vision/
GLFLAGS=-lm -lGL -lglut -lGLU -lsim -L../sim
CFLAGS=-O3 -Wall -I ../sim -I ../vision/lib -pthread -lncurses
CXXFLAGS=$(CVFLAGS) $(GLFLAGS) $(CFLAGS) -lui -L../../fpga/ui/

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
LIBVISION=../vision/libvision.a
LIBSIM=../sim/libsim.a
LIBUI=../../fpga/ui/libui.a
LIB=$(LIBVISION) $(LIBSIM) $(LIBUI)

AQUATUX=aquatux

parallel:
	make -j4 all

all: $(AQUATUX)

$(AQUATUX): $(OBJ) $(LIB)
	$(CC) -o $@ $(OBJ) $(LIB) $(CXXFLAGS)

$(LIBVISION):
	make -C ../vision `basename $@`

$(LIBSIM):
	make -C ../sim `basename $@`

$(LIBUI):
	make -C ../../fpga/ui `basename $@`

deps:
	$(CC) -MM $(CFLAGS) $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f $(OBJ)

.PHONY: parallel all deps clean
