include ../Makefile.in
include Makefile.deps

A=libvision.a
MAIN_DIR=main_src/
MAIN_SRC=$(MAIN_DIR)main.cpp
WEBCAM_SRC=$(MAIN_DIR)webcam.cpp
EXE=main
WEBCAM=webcam

all: $(EXE) $(WEBCAM) $(A)

$(EXE): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(CXXFLAGS) -I .

$(WEBCAM): $(OBJ) $(WEBCAM_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(WEBCAM_SRC) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

clean:
	rm -f $(OBJ) $(MAIN) $(WEBCAM) $(A) ./*~

.PHONY: parallel all deps clean
