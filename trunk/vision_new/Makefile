CC=g++
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=source/
OBJDIR=obj/

MGUI=$(OBJDIR)mgui.o
MV=$(OBJDIR)mv.o
MFILE=$(OBJDIR)mfile.o

INC=$(LIBDIR)mgui.h $(LIBDIR)mv.h 
OBJ=$(MGUI) $(MFILE) $(MV)

all: test

$(MGUI): $(INC) $(SRCDIR)mgui.cpp
	$(CC) -Wall -c $(SRCDIR)mgui.cpp -o $@ $(CVFLAGS)
$(MFILE): $(INC) $(SRCDIR)mfile.cpp
	$(CC) -Wall -c $(SRCDIR)mfile.cpp -o $@ $(CVFLAGS)
$(MV): $(INC) $(SRCDIR)mv.cpp
	$(CC) -Wall -c $(SRCDIR)mv.cpp -o $@ $(CVFLAGS)
$(OBJDIR)main.o: $(INC) main.cpp
	$(CC) -Wall -c main.cpp -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJDIR)main.o $(OBJ)
	$(CC) -Wall $(OBJDIR)main.o $(OBJ) -o $@ $(CVFLAGS)


clean:
	rm -f test ./obj/* ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

