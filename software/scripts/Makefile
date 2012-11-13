USE_C_SRC=1
include ../Makefile.in
include Makefile.deps

LIB=$(LIBUI)
EXES:=$(patsubst %.c,%.out,$(SRCS))

all: $(OBJ) $(EXES)
	echo $(EXES)

%.out: %.o $(LIB)
	$(CC) -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ) $(EXES)

.PHONY: parallel all deps clean
