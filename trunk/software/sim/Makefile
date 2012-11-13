SIM=sim
EXES=$(SIM)

include ../Makefile.in
include Makefile.deps

SIM_SRC=main_src/main.cpp
A=libsim.a

$(SIM): $(SIM_SRC) $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(SIM_SRC) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

.PHONY: parallel all deps clean
