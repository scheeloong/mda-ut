MAIN=main
WEBCAM=webcam
EXES=$(MAIN) $(WEBCAM)

include ../Makefile.in
include Makefile.deps

MAIN_DIR=main_src/
MAIN_SRC=$(MAIN_DIR)main.cpp
WEBCAM_SRC=$(MAIN_DIR)webcam.cpp
A=libvision.a

$(MAIN): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(CXXFLAGS) -I .

$(WEBCAM): $(OBJ) $(WEBCAM_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(WEBCAM_SRC) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

.PHONY: parallel all deps clean
