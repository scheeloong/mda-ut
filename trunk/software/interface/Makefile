AQUATUX=aquatux
EXES=$(AQUATUX)

include ../Makefile.in
include Makefile.deps

$(AQUATUX): $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(LIB) $(CXXFLAGS)

.PHONY: $(PHONY_TARGETS)
