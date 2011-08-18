CC=g++
FLAGS=-c -Wall 
CVFLAGS=`pkg-config opencv --cflags --libs`
LIBO= preproc_filters.o obj_detection.o calc_draw.o histograms.o

# each .o file depends on its .cpp file
.cpp.o:
	$(CC) $(FLAGS) $(CVFLAGS) $< -o $@

all: test_cv

# test_cv depends on the objects, no -c
test_cv: main.o cv_tasks.o $(LIBO)		
	$(CC) -Wall $(CVFLAGS) $(LIBO) main.o cv_tasks.o -o test_cv

cv_tasks.o: cv_tasks.cpp $(LIBO)
	$(CC) $(FLAGS) $(CVFLAGS) $< -o $@

all_but_main: cv_tasks.o $(LIBO)

clean:
	rm -rf $(LIBO) cv_tasks.o main.o test_cv

