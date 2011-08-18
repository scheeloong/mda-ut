FLAGS=-g -O2
all:  Makefile lzf.h  lzfP.h  lzf_c.o  lzf_d.o
	ar cru liblzf.a *.o

.c.o:	$<
	g++ -c $< $(FLAGS)
clean:
	rm -f *.o *.a
