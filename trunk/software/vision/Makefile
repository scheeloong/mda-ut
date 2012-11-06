CVFLAGS=`pkg-config --cflags --libs opencv`
CXXFLAGS=-O3 -Wall $(CVFLAGS)
A=libvision.a

SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
MAIN_DIR=main_src/
MAIN_SRC=$(MAIN_DIR)main.cpp
WEBCAM_SRC=$(MAIN_DIR)webcam.cpp

parallel:
	make -j4 all

all: main webcam $(A)

# this is the main executable and depends on each object
main: $(OBJ) $(MAIN_SRC)
	$(CXX) $(OBJ) $(MAIN_SRC) -o $@ $(CXXFLAGS) -I .

# webcam is another executable
webcam: $(OBJ) $(WEBCAM_SRC)
	$(CXX) $(OBJ) $(WEBCAM_SRC) -o $@ $(CXXFLAGS) -I .

# archive file
$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

deps:
	$(CC) -MM $(SRCS) > Makefile.deps

include Makefile.deps

clean:
	rm -f main webcam $(A) ./*~ $(OBJ)

.PHONY: parallel all deps clean
