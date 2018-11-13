# EPOS Utilities Makefile

include ../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc | grep -v test))

all:
	${MAKE} all_multithread -j$(NPROCS)

all_multithread: $(LIBUTIL)

$(LIBUTIL):	$(LIBUTIL)($(OBJS))


clean:
		$(CLEAN) *.o *_test
