# EPOS Tools Makefile

include	../makedefs

SUBDIRS := $(shell find * -maxdepth 0 -type d)

all:
	${MAKE} all_multithread -j$(NPROCS)

all_multithread: $(SUBDIRS)

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

clean:
		make MAKE:="${MAKE} -i clean" $(SUBDIRS)

FORCE:
