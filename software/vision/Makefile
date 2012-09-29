CC=g++ -O3 -Wall
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=src/
OBJDIR=obj/
A=libvision.a

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h $(LIBDIR)mvLines.h $(LIBDIR)mvCircles.h $(LIBDIR)mda_vision.h
OBJ=$(OBJDIR)mgui.o $(OBJDIR)mv.o $(OBJDIR)mvHSVFilter.o $(OBJDIR)mfile.o $(OBJDIR)mvLines.o $(OBJDIR)mvClustering.o $(OBJDIR)mvCircles.o
VOBJ=$(OBJDIR)mda_vision.o
TEST=$(OBJDIR)main.o
WEBCAM=$(OBJDIR)webcam.o

all: test webcam $(A)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -I$(LIBDIR) -c $< -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJ) $(VOBJ) $(TEST)
	$(CC) $(OBJ) $(VOBJ) $(TEST) -o $@ $(CVFLAGS)
# webcam is another executable
webcam: $(OBJ) $(VOBJ) $(WEBCAM)
	$(CC) $(OBJ) $(VOBJ) $(WEBCAM) -o $@ $(CVFLAGS)
# archive file
$(A): $(OBJ) $(VOBJ)
	ar -cvq $@ $(OBJ) $(VOBJ)

clean:
	rm -f test webcam ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~
	rm -f ./settings/*~

.PHONY: all clean
