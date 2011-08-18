OBJS= main.o bmp_io.o  site.o  glQuaternion.o \
	server.o  physical_model.o \
	virtual_io.o vision_wrapper.o 

EECG=-L/localdisk/glut/glut-3.7/lib/glut -L/usr/X11R6/lib -lXt -lSM -lICE -lXext -lX11 -lXi -lm
LIBS= $(EECG) -lGL -lglut -lGLU $(LIBPROTOCOL) $(LIBLZF) -lpthread $(LIBHOUGH) $(LIBFIFO) 
HAS_FIFO=0

FLAGS= -Wall -DHAS_FIFO=$(HAS_FIFO) `pkg-config opencv --cflags --libs`

INC=-I/localdisk/glut/glut-3.7/include -I$(LZFDIR) -I$(PROTOCOLDIR) -I$(HOUGHINC) -I$(SHAPEINC) -I$(FIFODIR)
INF=init.h  physical_model.h  planks.h  types.h

CVDIR=../vision
CVOBJS=$(CVDIR)/cv_tasks.o $(CVDIR)/calc_draw.o $(CVDIR)/preproc_filters.o $(CVDIR)/obj_detection.o

sim:	$(OBJS)
	make -C $(FIFODIR)
	make -C $(HOUGHDIR)
	make -C $(LZFDIR)
	make -C $(PROTOCOLDIR)
	make -C $(CVDIR) all_but_main
	g++ -Wall -o $@ $(OBJS) $(CVOBJS) $(LIBS)  $(QTL) `pkg-config opencv --cflags --libs`

include ../common/common.mk

.cpp.o:  $(INF)
	g++ -c $< $(FLAGS) $(INC) `pkg-config opencv --cflags --libs`

bmp_io.o: bmp_io.cpp
	g++ -c $< -g

planks:	planks.c
	g++  -o planks planks.c -g -Wall -lm

p: planks
	./planks > planks.h
	more planks.h

clean:
	rm -f $(OBJS) sim dep/*

vp:
	g++ -DPREVIEW -E protocol.cpp |grep -v "^$$"


dep/%.d: %.cpp
	$(SHELL) -ec 'g++ `pkg-config opencv --cflags --libs` -M $(INC) $< \
	| sed '\''s/\($(basename $<))\)\.o[ :]*/\1.o \/dep$(basename $<)) : /g'\'' > $@; [ -s $@ ] || rm -f $@'

include $(addprefix dep/,$(OBJS:.o=.d))

c:
	make clean
	make
	./sim i

