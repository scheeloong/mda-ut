CVFLAGS=`pkg-config --cflags --libs opencv` -lvision -L../vision/ -ltasks -L../tasks/
GLFLAGS=-lm -lGL -lglut -lGLU -lsim -L../sim
CFLAGS=-O3 -Wall -I ../sim -I ../vision/lib -I ../tasks/lib -pthread -lncurses
CXXFLAGS=$(CVFLAGS) $(GLFLAGS) $(CFLAGS) -lui -L../../fpga/ui/

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
A=libinterface.a
LIBVISION=../vision/libvision.a
LIBTASKS=../tasks/libtasks.a
LIBSIM=../sim/libsim.a
LIBUI=../../fpga/ui/libui.a
LIB=$(LIBVISION) $(LIBTASKS) $(LIBSIM) $(LIBUI)

AQUATUX=aquatux

parallel:
	make -j4 all

all: $(AQUATUX) $(A)

$(A): $(OBJ)
	ar -cvq $@ $(OBJ)

$(AQUATUX): $(OBJ) $(LIB)
	$(CC) -o $@ $(OBJ) $(LIB) $(CXXFLAGS)

$(LIBVISION):
	+make -C `dirname $@` `basename $@`

$(LIBTASKS):
	+make -C `dirname $@` `basename $@`

$(LIBSIM):
	+make -C `dirname $@` `basename $@`

$(LIBUI):
	+make -C `dirname $@` `basename $@`

deps:
	$(CC) -MM $(CFLAGS) $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f $(OBJ)

.PHONY: parallel all deps clean
