# EPOS Init Makefile

include ../../makedefs

OBJS := $(subst .cc,.o,$(shell find *.cc))

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

all_multithread: install

install:        $(MACH)_install_$(MODE)

$(MACH)_install_library: $(MACH)_init_library
		$(INSTALL) $(MACH)_init_first.o $(LIB)
		$(INSTALL) $(MACH)_init_system.o $(LIB)
		$(INSTALL) $(MACH)_init_application.o $(LIB)

$(MACH)_install_builtin: $(MACH)_init_builtin
		$(INSTALL) $(MACH)_init $(IMG)
		$(INSTALL) $(MACH)_init_application.o $(LIB)

$(MACH)_install_kernel: $(MACH)_init_kernel
		$(INSTALL) $(MACH)_init $(IMG)
		$(INSTALL) $(MACH)_init_application.o $(LIB)


$(MACH)_init_library: $(OBJS)
		$(INSTALL) init_first.o $(MACH)_init_first.o
		$(INSTALL) init_system.o $(MACH)_init_system.o
		$(INSTALL) init_application.o $(MACH)_init_application.o

$(MACH)_init_builtin: $(SRC)/system/$(MACH)_system $(OBJS) $(LIBINIT)
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start $(MACH_CODE_NAME)=$(INIT_ADDR) \
		--entry=_init -o $(MACH)_init \
		$(LIB)/$(ARCH)_crtbegin.o \
		init_first.o init_system.o \
		$(LIB)/$(ARCH)_crtend.o \
		-l$(LINIT) \
		-R $(SRC)/system/$(MACH)_system \
		-l$(LUTIL) -lgcc
		$(INSTALL) init_application.o $(MACH)_init_application.o

$(MACH)_init_kernel: $(SRC)/system/$(MACH)_system $(OBJS) $(LIBINIT)
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic \
		--section-start .init=$(INIT_ADDR) \
		--entry=_init -o $(MACH)_init \
		$(LIB)/$(ARCH)_crtbegin.o \
		init_first.o init_system.o \
		$(LIB)/$(ARCH)_crtend.o \
		-l$(LINIT) \
		-R $(SRC)/system/$(MACH)_system \
		-l$(LUTIL) -lgcc
		$(INSTALL) init_application.o $(MACH)_init_application.o


clean:
		$(CLEAN) *.o *_init

FORCE:
