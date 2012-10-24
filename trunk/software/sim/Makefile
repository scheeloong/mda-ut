OBJS= bmp_io.o site.o glQuaternion.o motors.o physical_model.o sim.o sim_cv.o
MAIN= main.o

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU -lvision
AFLAGS=-L$(CVDIR) $(CVDIR)$(CVA)

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=init.h sim.h physical_model.h planks.h types.h $(MOTORSDIR)/motors.h

CVDIR=../vision/
CVA=libvision.a

sim: $(MAIN) $(OBJS) $(CVDIR)$(CVA)
	g++ -O3 -o $@ $(OBJS) $(MAIN) $(LIBS) $(AFLAGS) $(FLAGS)

$(CVDIR)$(CVA):
	make -C $(CVDIR) $(CVA)

.cpp.o: $(INC)
	g++ -O3 -c $< $(FLAGS)

clean:
	rm -f $(OBJS) *.o sim *.bmp *~

.PHONY: clean
