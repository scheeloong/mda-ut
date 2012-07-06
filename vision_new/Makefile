CC=g++
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=source/
OBJDIR=obj/

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h 
OBJ=$(OBJDIR)mgui.o $(OBJDIR)main.o $(OBJDIR)mv.o $(OBJDIR)mfile.o

all: test

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CC) -Wall -I$(LIBDIR) -c $^ -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJ)
	$(CC) -Wall $(OBJ) -o $@ $(CVFLAGS)


clean:
	rm -f test ./obj/* ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: all clean
