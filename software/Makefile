DIRS = vision sim tasks ui scripts interface
TARGETS = parallel all deps clean

$(TARGETS):
	$(foreach dir,$(DIRS),$(MAKE) $@ -C $(dir) &&) echo "Make successful!"

.PHONY: $(TARGETS)
