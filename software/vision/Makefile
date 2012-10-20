CC=g++ -O3 -Wall
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=src/
OBJDIR=obj/
A=libvision.a

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h $(LIBDIR)mvLines.h $(LIBDIR)mvCircles.h $(LIBDIR)mda_vision.h $(LIBDIR)profile_bin.h
OBJ=$(OBJDIR)mgui.o $(OBJDIR)mv.o $(OBJDIR)mvHSVFilter.o $(OBJDIR)mvAdaptiveFilter.o $(OBJDIR)mfile.o $(OBJDIR)mvLines.o $(OBJDIR)mvClustering.o $(OBJDIR)mvCircles.o
VOBJ=$(OBJDIR)mda_vision.o
MAIN=$(OBJDIR)main.o
WEBCAM=$(OBJDIR)webcam.o
RANGE_TEST=$(OBJDIR)camera_range_test.o

all: main webcam camera_range_test $(A)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -I$(LIBDIR) -c $< -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
main: $(OBJ) $(VOBJ) $(MAIN)
	$(CC) $(OBJ) $(VOBJ) $(MAIN) -o $@ $(CVFLAGS)
# webcam is another executable
webcam: $(OBJ) $(VOBJ) $(WEBCAM)
	$(CC) $(OBJ) $(VOBJ) $(WEBCAM) -o $@ $(CVFLAGS)
# range_test executable
camera_range_test: $(OBJ) $(VOBJ) $(RANGE_TEST)
	$(CC) $(OBJ) $(VOBJ) $(RANGE_TEST) -o $@ $(CVFLAGS)
# archive file
$(A): $(OBJ) $(VOBJ)
	ar -cvq $@ $(OBJ) $(VOBJ)

clean:
	rm -f main webcam camera_range_test ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~
	rm -f ./settings/*~

.PHONY: all clean
