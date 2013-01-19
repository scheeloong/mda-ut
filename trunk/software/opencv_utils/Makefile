SRCS:=$(wildcard *.cpp)
OBJ:=$(patsubst %.cpp,%.o,$(SRCS))
EXES:=$(patsubst %.cpp,%.out,$(SRCS))

include ../Makefile.in
include Makefile.deps

%.out: %.o $(LIB)
	$(CC) -o $@ $< $(CXXFLAGS)

.PHONY: parallel all deps clean
