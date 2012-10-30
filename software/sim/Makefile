SRCS:=$(wildcard *.cpp)
OBJS= bmp_io.o site.o glQuaternion.o motors.o physical_model.o sim.o sim_cv.o
MAIN= main.o
EXE= sim
A=libsim.a

LIBS= -lm -lGL -lglut -lGLU

CXXFLAGS = -O3 -Wall `pkg-config opencv --cflags --libs`

INC=init.h sim.h physical_model.h planks.h types.h $(MOTORSDIR)/motors.h

parallel:
	make -j4 all

all: $(EXE) $(A)

$(EXE): $(MAIN) $(OBJS) $(CVDIR)$(CVA)
	g++ -O3 -o $@ $(OBJS) $(MAIN) $(LIBS) $(CXXFLAGS)

$(A): $(OBJS)
	ar -cvq $@ $(OBJS)

deps:
	$(CC) -MM $(CXXFLAGS) $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f $(OBJS) $(EXE) $(A) *.bmp *~

.PHONY: parallel all deps clean
