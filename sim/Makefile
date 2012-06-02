OBJS= main.o bmp_io.o  site.o  glQuaternion.o physical_model.o keyboard.o

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU -lmotors -lvision
AFLAGS=-L$(CVDIR) $(CVDIR)$(CVA) -L$(MOTORSDIR) $(MOTORSDIR)$(MOTORSA)

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=init.h  physical_model.h planks.h types.h $(MOTORSDIR)/motors.h $(CVDIR)/cv_tasks.h

MOTORSDIR=../motors/
MOTORSA=libmotors.a

CVDIR=../vision/
CVA=libvision.a

sim:	$(OBJS) $(CVDIR)$(CVA) $(MOTORSDIR)$(MOTORSA)
	g++ -o $@ $(OBJS) $(LIBS) $(AFLAGS) $(FLAGS)

$(CVDIR)$(CVA):
	make -C $(CVDIR) $(CVA)

$(MOTORSDIR)$(MOTORSA):
	make -C $(MOTORSDIR) $(MOTORSA)

.cpp.o:  $(INC)
	g++ -c $< $(FLAGS)

clean:
	rm -f $(OBJS) *.o sim *.bmp *~

.PHONY: clean
