include ../Makefile.in
include Makefile.deps

A=libtasks.a
MAIN_SRC=main_src/main.cpp
MAIN=main

all: $(MAIN) $(A)

$(MAIN): $(OBJ) $(MAIN_SRC) $(LIB)
	$(CXX) -o $@ $(OBJ) $(MAIN_SRC) $(LIB) $(CXXFLAGS) -I .

$(A): $(OBJ)
	rm -f $@
	ar -cvq $@ $(OBJ)

clean:
	rm -f $(OBJ) $(MAIN) $(A) ./*~

.PHONY: parallel all deps clean

