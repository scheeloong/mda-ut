MAIN=main
WEBCAM=webcam
HU=hu
EXES=$(MAIN) $(WEBCAM) $(HU)
A=libvision.a

include ../Makefile.in
include Makefile.deps

MAIN_DIR=main_src/
MAIN_SRC=$(MAIN_DIR)main.cpp
WEBCAM_SRC=$(MAIN_DIR)webcam.cpp
HU_SRC=$(MAIN_DIR)hu.cpp

$(MAIN): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(CXXFLAGS) -I .

$(WEBCAM): $(OBJ) $(WEBCAM_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(WEBCAM_SRC) $(CXXFLAGS) -I .

$(HU): $(OBJ) $(HU_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(HU_SRC) $(CXXFLAGS) -I .

.PHONY: parallel all deps clean
