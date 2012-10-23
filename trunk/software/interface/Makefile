CC=g++ -O3 -Wall -I ../sim

INC=*.h
OBJ=AttitudeInputSimulator.o AttitudeInputSubmarine.o SimulatorSingleton.o SubmarineSingleton.o

all: $(OBJ)

%.o: %.cpp $(INC)
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ)

.PHONY: all clean
