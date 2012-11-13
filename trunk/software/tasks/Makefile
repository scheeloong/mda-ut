MAIN=main
EXES=$(MAIN)

include ../Makefile.in
include Makefile.deps

MAIN_SRC=main_src/main.cpp
A=libtasks.a

$(MAIN): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(LIB) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

.PHONY: parallel all deps clean

