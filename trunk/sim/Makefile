OBJS= main.o bmp_io.o  site.o  glQuaternion.o physical_model.o keyboard.o

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU -lmission -lvision
AFLAGS=-L$(CVDIR) $(CVDIR)$(CVA) -L$(MISSIONDIR) $(MISSIONDIR)$(MISSIONA)

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=init.h  physical_model.h planks.h types.h $(MISSIONDIR)/mission.h $(CVDIR)/cv_tasks.h

MISSIONDIR=../mission/
MISSIONA=libmission.a

CVDIR=../vision/
CVA=libvision.a

sim:	$(OBJS) $(CVDIR)$(CVA) $(MISSIONDIR)$(MISSIONA)
	g++ -o $@ $(OBJS) $(LIBS) $(AFLAGS) $(FLAGS)

$(CVDIR)$(CVA):
	make -C $(CVDIR) $(CVA)

$(MISSIONDIR)$(MISSIONA):
	make -C $(MISSIONDIR) $(MISSIONA)

.cpp.o:  $(INC)
	g++ -c $< $(FLAGS)

clean:
	rm -f $(OBJS) sim *.bmp

.PHONY: clean $(CVDIR)$(CVA) $(MISSIONDIR)$(MISSIONA) sim
