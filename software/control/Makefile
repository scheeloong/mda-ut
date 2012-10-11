CC=g++ -O3 -Wall
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBDIR=lib/
SRCDIR=src/
OBJDIR=obj/
A=libcontrol.a

INC=$(LIBDIR)mda_control.h $(LIBDIR)vci.h
OBJ=$(OBJDIR)mda_control.o
TEST=$(OBJDIR)main.o

all: test $(A)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC)
	$(CC) -I$(LIBDIR) -c $< -o $@ $(CVFLAGS)

# test is the main executable and depends on each object
test: $(OBJ) $(TEST)
	$(CC) $(OBJ) $(TEST) -o $@ 

# archive file
$(A): $(OBJ)
	ar -cvq $@ $(OBJ)

clean:
	rm -f test ./obj/* $(A) ./*~
	rm -f $(LIBDIR)*~
	rm -f $(SRCDIR)*~

.PHONY: all clean

