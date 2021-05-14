TARGET = fibo
TOP_DIR = $(shell pwd)
INCLUDE = -I$(TOP_DIR)/include
SUBDIRS = code

export INCLUDE

.PHONY: clean $(SUBDIRS)

$(TARGET): $(SUBDIRS)
	gcc -Wall -O3 -s -std=c99 -o $@ `cat objects`
	rm objects
$(SUBDIRS):
	make -C $@
clean:
	for d in $(SUBDIRS); do make -C $$d clean; done
