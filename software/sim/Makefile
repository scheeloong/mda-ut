include ../Makefile.in
include Makefile.deps

A=libsim.a
SIM_SRC=main_src/main.cpp
EXE=sim

all: $(EXE) $(A)

$(EXE): $(SIM_SRC) $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(SIM_SRC) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

clean:
	rm -f $(OBJ) $(EXE) $(A) *.bmp ./*~

.PHONY: parallel all deps clean
