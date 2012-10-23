DIRS = sim motors vision control scripts interface

all:
	for dir in $(DIRS); do make -C $$dir; done
clean:
	for dir in $(DIRS); do make $@ -C $$dir; done

.PHONY: all clean $(DIRS)
