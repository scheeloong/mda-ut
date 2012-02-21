OBJS= main.o bmp_io.o  site.o  glQuaternion.o physical_model.o keyboard.o

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=init.h  physical_model.h planks.h types.h $(MISSIONDIR)/mission.h $(CVDIR)/cv_tasks.h

MISSIONDIR=../mission
MISSIONOBJS=$(MISSIONDIR)/mission.o

CVDIR=../vision
CVOBJS=$(CVDIR)/common.o $(CVDIR)/task_gate.o $(CVDIR)/task_path.o $(CVDIR)/task_U.o $(CVDIR)/calc_draw.o $(CVDIR)/preproc_filters.o $(CVDIR)/obj_detection.o

sim:	$(OBJS) $(CVOBJS)
	make -C $(CVDIR) #all_but_main
	make -C $(MISSIONDIR)
	g++ -o $@ $(OBJS) $(CVOBJS) $(MISSIONOBJS) $(FLAGS) $(LIBS) $(QTL)

.cpp.o:  $(INC)
	g++ -c $< $(FLAGS)

clean:
	rm -f $(OBJS) sim *.bmp

