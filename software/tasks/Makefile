CC=g++ -O3 -Wall
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
VLIBDIR=../vision/lib/
SRCDIR=src/
OBJDIR=obj/
A=libtasks.a

CVDIR=../vision/
CVA=libvision.a
AFLAGS=-L$(CVDIR) $(CVDIR)$(CVA)

INC=$(LIBDIR)mda_vision.h $(LIBDIR)mda_control.h $(LIBDIR)vci.h
VOBJ=$(OBJDIR)test_vision.o $(OBJDIR)gate_vision.o $(OBJDIR)path_vision.o $(OBJDIR)buoy_vision.o $(OBJDIR)frame_vision.o
COBJ=
MAIN=$(OBJDIR)main.o

parallel:
	make -j4 all

all: main $(A)

$(CVDIR)$(CVA):
	make -C $(CVDIR) $(CVA)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -I$(LIBDIR) -I$(VLIBDIR) -c $< -o $@ $(CVFLAGS)

# the main executable
main: $(VOBJ) $(COBJ) $(MAIN)
	$(CC) $(VOBJ) $(COBJ) $(MAIN) -o $@ $(AFLAGS) $(CVFLAGS)

# archive file
$(A): $(OBJ)
	ar -cvq $@ $(OBJ)

clean:
	rm -f main ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: parallel all clean

