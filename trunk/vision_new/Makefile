CC=g++ -O3
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=src/
OBJDIR=obj/
A=libvision.a

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h $(LIBDIR)mvLines.h $(LIBDIR)mvCircles.h
OBJ=$(OBJDIR)mgui.o $(OBJDIR)mv.o $(OBJDIR)mvHSVFilter.o $(OBJDIR)mfile.o $(OBJDIR)mvLines.o $(OBJDIR)mvClustering.o $(OBJDIR)mvCircles.o
TEST=$(OBJDIR)main.o
WEBCAM=$(OBJDIR)webcam.o

all: test webcam $(A)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -Wall -I$(LIBDIR) -c $< -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJ) $(TEST)
	$(CC) -Wall $(OBJ) $(TEST) -o $@ $(CVFLAGS)
# webcam is another executable
webcam: $(OBJ) $(WEBCAM)
	$(CC) -Wall $(OBJ) $(WEBCAM) -o $@ $(CVFLAGS)
# archive file
$(A): $(OBJ)
	ar -cvq $@ $(OBJ)

clean:
	rm -f test webcam ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: all clean
