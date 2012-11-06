DIRS = vision sim tasks scripts interface
TARGETS = parallel all deps clean

$(TARGETS):
	for dir in $(DIRS); do make $@ -C $$dir; done

.PHONY: $(TARGETS)
