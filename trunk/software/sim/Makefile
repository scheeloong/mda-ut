LFLAGS= -lm -lGL -lglut -lGLU
CXXFLAGS = -O3 -Wall $(LFLAGS) `pkg-config opencv --cflags --libs`
A=libsim.a

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
SIM_SRC=main_src/main.cpp
EXE=sim

parallel:
	make -j4 all

all: $(EXE) $(A)

$(EXE): $(SIM_SRC) $(OBJ) $(CVDIR)$(CVA)
	g++ -o $@ $(OBJ) $(SIM_SRC) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

deps:
	$(CC) -MM $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f $(OBJ) $(EXE) $(A) *.bmp *~

.PHONY: parallel all deps clean
