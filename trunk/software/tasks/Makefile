CC=g++ -O3 -Wall -g
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

INC=$(LIBDIR)mda_vision.h $(LIBDIR)mda_tasks.h $(LIBDIR)Mission.h
VOBJ=$(OBJDIR)vision_test.o $(OBJDIR)vision_gate.o $(OBJDIR)vision_path.o $(OBJDIR)vision_buoy.o $(OBJDIR)vision_frame.o
COBJ=$(OBJDIR)Mission.o $(OBJDIR)task_test.o
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

