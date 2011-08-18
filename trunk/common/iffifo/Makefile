
OBJ=iffifo.o


libfifo.a: $(OBJ)
	ar cru $@ $(OBJ)

all: client server

client: fifoclient.c $(OBJ)
	g++ -o $@ fifoclient.c $(OBJ) -g -Wall
server: fifoserver.c $(OBJ)
	g++ -o $@ fifoserver.c $(OBJ) -g -Wall

.c.o:
	g++ -g -Wall -O2 -c $<

clean:
	rm -f *.o *.a
