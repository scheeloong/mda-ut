CC=g++
FLAGS=-c -Wall $(CVFLAGS)
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBO=preproc_filters.o obj_detection.o calc_draw.o histograms.o main.o
TASKS=common.o task_gate.o task_path.o task_buoy.o task_U.o
INC= calc_draw.h cv_tasks.h histograms.h obj_detection.h preproc_filters.h
MISSIONDIR=../mission/
MISSIONA=libmission.a
A=libvision.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ $(FLAGS)

all: test_cv

# test_cv depends on the objects, no -c
test_cv: $(TASKS) $(LIBO) $(MISSIONDIR)$(MISSIONA)
	$(CC) $(LIBO) $(TASKS) -o $@ -L$(MISSIONDIR) -lmission -Wall $(CVFLAGS)

$(MISSIONDIR)$(MISSIONA):
	make -C $(MISSIONDIR) $(MISSIONA)

$(A): $(TASKS) $(LIBO)
	ar -cvq $@ $(LIBO) $(TASKS)

clean:
	rm -rf $(LIBO) $(TASKS) $(A) test_cv ./*~

.PHONY: all clean
