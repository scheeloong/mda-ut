CVFLAGS=`pkg-config --cflags --libs opencv`
LFLAGS=-lm -lGL -lglut -lGLU -lsim -L../sim -lncurses -lui -L../../fpga/ui/ -lvision -L../vision/ -ltasks -L../tasks/ -pthread
CFLAGS=-O3 -Wall -I ../sim -I ../vision -I ../tasks
CXXFLAGS=$(CFLAGS) $(LFLAGS) $(CVFLAGS)

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
LIBVISION=../vision/libvision.a
LIBTASKS=../tasks/libtasks.a
LIBSIM=../sim/libsim.a
LIBUI=../../fpga/ui/libui.a
LIB=$(LIBVISION) $(LIBTASKS) $(LIBSIM) $(LIBUI)

AQUATUX=aquatux

parallel:
	make -j4 all

all: $(AQUATUX)

$(AQUATUX): $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(LIB) $(CXXFLAGS)

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
	rm -f $(OBJ) $(AQUATUX)

.PHONY: parallel all deps clean
