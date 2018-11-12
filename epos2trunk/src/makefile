# EPOS Source Makefile

include	../makedefs

SUBDIRS := utility architecture machine component setup boot system init

all:		$(SUBDIRS)

$(SUBDIRS):	FORCE
		(cd $@ && $(MAKE))

clean:
	${MAKE} clean_multithread -j$(NPROCS)

clean_multithread:
		make MAKE:="$(MAKECLEAN)" $(SUBDIRS)

FORCE:
