DIRS = vision sim tasks scripts interface
TARGETS = parallel all deps clean

$(TARGETS):
	$(foreach dir,$(DIRS),$(MAKE) $@ -C $(dir);)

.PHONY: $(TARGETS)
