# EPOS Setup Makefile

include ../../makedefs

TARGET := $(if $(shell find $(MACH)* 2> /dev/null), $(if $(SETUP_ADDR), install, install.o))

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

all_multithread: $(TARGET)

ifeq ($(MACH),cortex)
$(MACH)_setup.s: $(MACH)_setup.S
		$(CPP) $(CPPFLAGS) -D $(MMOD) $< -o $@

$(MACH)_setup.o: $(MACH)_setup.s
		sed -e 's/MEM_TOP/$(call GETTK,MEM_TOP,$(MACH_TRAITS))/' -i $<
		$(AS) $(ASFLAGS) -o $@ $<
endif

$(MACH)_setup:	$(MACH)_setup.o
		$(LD) $(LDFLAGS) -L$(CCLIB) --omagic --section-start .init=$(SETUP_ADDR) -o $@ $^ -l$(LINIT) -l$(LMACH) -l$(LARCH) -l$(LUTIL) -lgcc

install:	$(MACH)_setup
		$(INSTALL) $< $(IMG)

install.o:	$(MACH)_setup.o
		$(INSTALL) $< $(LIB)

clean:
		$(CLEAN) *.o *.s *_setup
