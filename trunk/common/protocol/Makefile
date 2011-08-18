FLAGS=-g -O2 -Wall -I../lzf
INC=  csocket.h protocol.h machine.h protocol.def
all:	Makefile protocol.o  \
	csocket.o  \
	client_routines.o server_routines.o
	ar cru libprotocol.a *.o


.cpp.o:	$<  $(INC)
	g++ -c $< $(FLAGS)

clean:
	rm -f *.o *.a
