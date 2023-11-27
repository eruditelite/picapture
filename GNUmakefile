################################################################################
# GNUmakefile
#
# Makefile for all the scripts...
################################################################################

TAG = $(shell git describe --tags --always --dirty)

%.py: %.py.in
	rm -f $@
	sed -e "s/__VERSION__/\"${TAG}\"/" $^ >$@
	chmod 544 $@

%.py.d: %.py.in
	@echo "$(shell echo $@ | sed -e 's/.d//') : $^" >$@

PYTHON_IN := $(shell find . -name '*.py.in')
TARGETS = $(PYTHON_IN:.py.in=.py)
deps = $(PYTHON_IN:.py.in=.py.d)

all: $(TARGETS)

clean:
	rm -f *~ $(TARGETS) *.d

-include $(deps)
