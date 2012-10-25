OBJS= bmp_io.o site.o glQuaternion.o motors.o physical_model.o sim.o sim_cv.o
MAIN= main.o
EXE= sim
A=libsim.a

LIBS= -lm -lGL -lglut -lGLU -lvision
AFLAGS=-L$(CVDIR) $(CVDIR)$(CVA)

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=init.h sim.h physical_model.h planks.h types.h $(MOTORSDIR)/motors.h

CVDIR=../vision/
CVA=libvision.a

all: $(EXE) $(A)

$(EXE): $(MAIN) $(OBJS) $(CVDIR)$(CVA)
	g++ -O3 -o $@ $(OBJS) $(MAIN) $(LIBS) $(AFLAGS) $(FLAGS)

$(CVDIR)$(CVA):
	make -C $(CVDIR) $(CVA)

.cpp.o: $(INC)
	g++ -O3 -c $< $(FLAGS)

$(A): $(OBJS)
	ar -cvq $@ $(OBJS)

clean:
	rm -f $(OBJS) $(EXE) $(A) *.bmp *~

.PHONY: all clean
