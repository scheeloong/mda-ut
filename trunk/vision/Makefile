CC=g++
FLAGS=-c -Wall $(CVFLAGS)
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBO=preproc_filters.o obj_detection.o calc_draw.o histograms.o
TASKS=common.o task_path.o task_buoy.o
INC= calc_draw.h cv_tasks.h histograms.h obj_detection.h preproc_filters.h

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ $(FLAGS)

all: test_cv

# test_cv depends on the objects, no -c
test_cv: main.o $(TASKS) $(LIBO)		
	make -C ../mission
	$(CC) $(LIBO) $(TASKS) main.o ../mission/mission.o -o $@ -Wall $(CVFLAGS)

#cv_tasks.o: cv_tasks.cpp cv_tasks.h $(LIBO)
#	$(CC) $< -o $@ $(FLAGS)
common.o: common.cpp common.h $(LIBO)
	$(CC) $< -o $@ $(FLAGS)
task_gate.o: task_gate.cpp task_gate.h $(LIBO)
	$(CC) $< -o $@ $(FLAGS)
task_path.o: task_path.cpp task_path.h $(LIBO)
	$(CC) $< -o $@ $(FLAGS)
task_buoy.o: task_buoy.cpp task_buoy.h $(LIBO)
	$(CC) $< -o $@ $(FLAGS)

all_but_main: $(TASKS) $(LIBO)

clean:
	rm -rf $(LIBO) $(TASKS) main.o test_cv ./*~

.PHONY: all all_but_main clean
