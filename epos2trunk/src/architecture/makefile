# EPOS Architecture Mediators Makefile

include	../../makedefs

ifeq ($(MAKE),$(MAKECLEAN))
SUBDIRS := $(shell find * -maxdepth 0 -type d)
else
SUBDIRS := $(ARCH)
endif

# How can I write a makefile to auto-detect and parallelize the build with GNU Make?
# https://stackoverflow.com/questions/2527496/how-can-i-write-a-makefile-to-auto-detect-and-parallelize-the-build-with-gnu-mak
ifeq ($J,)
	ifeq ($(OS),Windows_NT)
		NPROCS := $(NUMBER_OF_PROCESSORS)
		FIND_EXEC := /bin/find
	else
		ifeq ($(UNAME),Darwin)
		  NPROCS := $(shell sysctl -n hw.ncpu)
		  FIND_EXEC := /usr/bin/find
		else
		  NPROCS := $(shell grep -c ^processor /proc/cpuinfo)
		  FIND_EXEC := /usr/bin/find
		endif
	endif
else
  NPROCS := ""
endif

all:
	${MAKE} all_multithread -j$(NPROCS)

all_multithread: $(SUBDIRS)

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

clean:
		make MAKE:="$(MAKECLEAN)" $(SUBDIRS)

FORCE:
