OBJS= main.o bmp_io.o  site.o  glQuaternion.o physical_model.o

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU -lpthread

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INF=init.h  physical_model.h planks.h types.h

CVDIR=../vision
CVOBJS=$(CVDIR)/cv_tasks.o $(CVDIR)/calc_draw.o $(CVDIR)/preproc_filters.o $(CVDIR)/obj_detection.o

sim:	$(OBJS)
	make -C $(CVDIR) all_but_main
	g++ -o $@ $(OBJS) $(CVOBJS) $(FLAGS) $(LIBS) $(QTL)

.cpp.o:  $(INF)
	g++ -c $< $(FLAGS) $(INC)

clean:
	rm -f $(OBJS) sim *.bmp

