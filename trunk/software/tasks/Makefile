CVFLAGS=`pkg-config --cflags --libs opencv`
CFLAGS=-O3 -Wall -I ../vision/ -I ../sim/ -I ../tasks/
CXXFLAGS=$(CFLAGS) $(CVFLAGS)
A=libtasks.a

LIBVISION=../vision/libvision.a
LIB=$(LIBVISION)

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
MAIN_SRC=main_src/main.cpp

parallel:
	make -j4 all

all: main $(A)

$(LIBVISION):
	+make -C `dirname $@` `basename $@`

# the main executable
main: $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) $(OBJ) $(MAIN_SRC) $(LIB) -o $@ $(CXXFLAGS) -I .

# archive file
$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

deps:
	$(CC) -MM $(CFLAGS) $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f main $(A) ./*~ $(OBJ)

.PHONY: parallel all deps clean

