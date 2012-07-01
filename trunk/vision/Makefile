CC=g++
FLAGS=-c -Wall $(CVFLAGS)
CVFLAGS=`pkg-config --cflags --libs opencv`
LIBO=preproc_filters.o obj_detection.o calc_draw.o histograms.o common.o
TASKS=task_gate.o task_path.o task_buoy.o task_U.o task_torp.o
INC= calc_draw.h cv_tasks.h histograms.h obj_detection.h preproc_filters.h
MISSIONDIR=../motors/
MISSIONA=libmotors.a
A=libvision.a

# each .o file depends on its .cpp file
.cpp.o: $(INC)
	$(CC) $< -o $@ $(FLAGS)

all: webcam $(A)

# webcam doesnt depend on tasks
webcam: webcam.o $(LIBO) $(TASKS) $(MISSIONDIR)$(MISSIONA)
	$(CC) webcam.o $(LIBO) $(TASKS) -o $@ -L$(MISSIONDIR) -lmotors -Wall $(CVFLAGS)
	rm -f *.a
# test_cv depends on the objects, no -c
#test_cv: main.o $(TASKS) $(LIBO) $(MISSIONDIR)$(MISSIONA)
#	$(CC) main.o $(LIBO) $(TASKS) -o $@ -L$(MISSIONDIR) -lmotors -Wall $(CVFLAGS)
#	rm -f *.a

$(MISSIONDIR)$(MISSIONA):
	make -C $(MISSIONDIR) $(MISSIONA)

$(A): $(TASKS) $(LIBO)
	ar -cvq $@ $(LIBO) $(TASKS)

clean:
	rm -rf $(LIBO) $(TASKS) $(A) webcam ./*~

.PHONY: all clean
