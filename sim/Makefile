OBJS= main.o bmp_io.o  site.o  glQuaternion.o \
	server.o  physical_model.o \
	virtual_io.o vision_wrapper.o 

EECG=-lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU $(LIBPROTOCOL) $(LIBLZF) -lpthread $(LIBHOUGH) $(LIBFIFO) 

FLAGS= -Wall `pkg-config opencv --cflags --libs`

INC=-I$(LZFDIR) -I$(PROTOCOLDIR) -I$(HOUGHINC) -I$(SHAPEINC) -I$(FIFODIR)
INF=init.h  physical_model.h planks.h types.h

CVDIR=../vision
CVOBJS=$(CVDIR)/cv_tasks.o $(CVDIR)/calc_draw.o $(CVDIR)/preproc_filters.o $(CVDIR)/obj_detection.o

sim:	$(OBJS)
	make -C $(FIFODIR)
	make -C $(HOUGHDIR)
	make -C $(LZFDIR)
	make -C $(PROTOCOLDIR)
	make -C $(CVDIR) all_but_main
	g++ -o $@ $(OBJS) $(CVOBJS) $(FLAGS) $(LIBS) $(QTL)

include ../common/common.mk

.cpp.o:  $(INF)
	g++ -c $< $(FLAGS) $(INC)

bmp_io.o: bmp_io.cpp
	g++ -c $< -g

clean:
	rm -f $(OBJS) sim *.bmp

