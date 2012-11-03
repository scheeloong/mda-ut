CC=g++ -O3 -Wall
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
VLIBDIR=../vision/lib/
INTERDIR=../interface/
SIMDIR=../sim/
SRCDIR=src/
OBJDIR=obj/
A=libtasks.a

LIBVISION=../vision/libvision.a
LIB=$(LIBVISION)

INC=$(LIBDIR)mda_vision.h $(LIBDIR)mda_tasks.h $(LIBDIR)vci.h $(LIBDIR)Mission.h
VOBJ=$(OBJDIR)test_vision.o $(OBJDIR)gate_vision.o $(OBJDIR)path_vision.o $(OBJDIR)buoy_vision.o $(OBJDIR)frame_vision.o
COBJ=$(OBJDIR)Mission.o $(OBJDIR)test_task.o
MAIN=$(OBJDIR)main.o

parallel:
	make -j4 all

all: main $(A)

$(LIBVISION):
	+make -C `dirname $@` `basename $@`

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -I$(LIBDIR) -I$(VLIBDIR) -I$(INTERDIR) -I$(SIMDIR) -c $< -o $@ $(CVFLAGS)

# the main executable
main: $(VOBJ) $(COBJ) $(MAIN) $(LIB)
	$(CC) $(VOBJ) $(COBJ) $(MAIN) -o $@ $(LIB) $(CVFLAGS)

# archive file
$(A): $(VOBJ) $(COBJ)
	rm -f $@
	ar -cvq $@ $(VOBJ) $(COBJ)

clean:
	rm -f main ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: parallel all clean

