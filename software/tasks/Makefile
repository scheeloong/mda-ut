MAIN=main
EXES=$(MAIN)
A=libtasks.a

include ../Makefile.in
include Makefile.deps

MAIN_SRC=main_src/main.cpp

$(MAIN): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(LIB) $(CXXFLAGS) -I .

.PHONY: $(PHONY_TARGETS)
