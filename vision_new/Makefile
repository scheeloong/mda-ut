CC=g++
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=src/
OBJDIR=obj/

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h 
OBJ=$(OBJDIR)mgui.o $(OBJDIR)mv.o $(OBJDIR)mvGradient.o $(OBJDIR)mvHSVFilter.o $(OBJDIR)mfile.o $(OBJDIR)main.o

all: test

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -Wall -I$(LIBDIR) -c $< -o $@ $(CVFLAGS)

#$(OBJDIR)mfile.o: $(SRCDIR)mfile.cpp $(LIBDIR)mgui.h
#	$(CC) -Wall -I$(LIBDIR) -c $^ -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJ)
	$(CC) -Wall $(OBJ) -o $@ $(CVFLAGS)


clean:
	rm -f test ./obj/* ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: all clean
