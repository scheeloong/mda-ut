include ../Makefile.in
include Makefile.deps

AQUATUX=aquatux

all: $(AQUATUX)

$(AQUATUX): $(OBJ) $(LIB)
	$(CXX) -o $@ $(OBJ) $(LIB) $(CXXFLAGS)

clean:
	rm -f $(OBJ) $(AQUATUX) ./*~

.PHONY: parallel all deps clean
