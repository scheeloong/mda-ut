SIM=sim
EXES=$(SIM)
A=libsim.a

include ../Makefile.in
include Makefile.deps

SIM_SRC=main_src/main.cpp

$(SIM): $(SIM_SRC) $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(SIM_SRC) $(CXXFLAGS) -I .

.PHONY: $(PHONY_TARGETS)
