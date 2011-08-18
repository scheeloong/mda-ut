SRCP= main.cpp   test/rif.cpp 
SRCPP=test/save.cpp draw_line.cpp
SRCC= hough.c peak.c lines.c thin.c process.c detection.c blob.c
SRCS= shapes/cleanup.c shapes/line.c shapes/main.c
OBJC=$(SRCC:.c=.o) $(SRCS:.c=.o) 
OBJPP=$(SRCPP:.cpp=.o)
OBJCPP=$(SRCP:.cpp=.o) $(OBJPP) 
OBJS= $(OBJCPP) $(OBJC)

lib:	$(OBJC) $(OBJPP)
	ar cru libhough.a $(OBJC) $(OBJPP)

include ../common.mk


ifdef MDA_SUB
QT4=1
FLAGS=-Wall -O2 
else
FLAGS=-Wall -O2 -g
endif

.cpp.o:
	g++  -c $<  $(QTI) -o $@ $(FLAGS) -DQT4=$(QT4)

.c.o:
	g++  -c $<  $(QTI) -o $@ $(FLAGS) -DQT4=$(QT4)


pic2lines: $(OBJS)
	g++ -o $@ $(OBJS) -g -Wall -O2 $(QTL)  $(GSL) 


verif: verif.c
	g++ -o verif -g -Wall verif.c -lm

clean:
	rm -f *.o pic2lines *.bmp *.rif *.a test/*.o shapes/*.o

S2=detection.cpp rgb.cpp
rgb: $(S2)
	g++ -o $@ $(S2) -g -Wall -O2 -lm


thin: thin.c
	g++ -o $@ thin.c -g -Wall -O2 

mx: mx.c
	g++ -g -O2 -Wall -o mx mx.c


tlines: lines.c
	g++ -g -O2 -Wall -o tlines lines.c -DTONLY

