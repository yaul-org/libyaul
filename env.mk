ifneq (1,$(words [$(shell pwd)]))
  $(error Current directory ($(shell pwd)) contains spaces)
endif

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifneq (1,$(words [$(strip $(INSTALL_ROOT))]))
  $(error INSTALL_ROOT (install root directory) contains spaces)
endif

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifneq (1,$(words [$(strip $(BUILD_ROOT))]))
  $(error BUILD_ROOT (build root directory) contains spaces)
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

ifneq (1,$(words [$(strip $(BUILD))]))
  $(error BUILD (build directory) contains spaces)
endif

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
endif

ifeq '$(OS)' "Windows_NT"
EXE_EXT:= .exe
endif

# Installation location; by default, this is the same path that
# contains the toolchain, but it can be set to another path instead.
PREFIX ?= $(INSTALL_ROOT)

SH_ARCH:= sh-elf

SH_AS:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-as$(EXE_EXT)
SH_AR:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-ar$(EXE_EXT)
SH_CC:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-gcc$(EXE_EXT)
SH_CXX:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-g++$(EXE_EXT)
SH_LD:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-gcc$(EXE_EXT)
SH_NM:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-nm$(EXE_EXT)
SH_OBJCOPY:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-objcopy$(EXE_EXT)
SH_OBJDUMP:= $(INSTALL_ROOT)/$(SH_ARCH)/bin/$(SH_ARCH)-objdump$(EXE_EXT)

SH_CFLAGS_shared:= \
	-pedantic \
	-s \
	-ffreestanding \
	-ffast-math \
	-fstrict-aliasing \
	-fomit-frame-pointer \
	-Wall \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses \
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE) \
	-DFIXMATH_NO_OVERFLOW=1 \
	-DFIXMATH_NO_ROUNDING=1

SH_CFLAGS:= \
	-std=c11 \
	$(SH_CFLAGS_shared)

SH_CXXFLAGS_shared:=

SH_CXXFLAGS:= \
	-fno-exceptions \
	-fno-rtti \
	-fno-unwind-tables \
	-fno-threadsafe-statics \
	-fno-use-cxa-atexit \
	$(SH_CXXFLAGS_shared)

SH_CFLAGS_shared_release:= -O2
SH_CFLAGS_shared_debug:= -O0 -g -DDEBUG

SH_CFLAGS_release:= $(SH_CFLAGS_shared_release) $(SH_CFLAGS)
SH_CFLAGS_debug:= $(SH_CFLAGS_shared_debug) $(SH_CFLAGS)

SH_CXXFLAGS_release:= $(SH_CFLAGS_shared_release) $(SH_CXXFLAGS)
SH_CXXFLAGS_debug:= $(SH_CFLAGS_shared_debug) $(SH_CXXFLAGS)

define macro-sh-build-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)$(SH_CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(SH_CFLAGS_$1) \
		$(foreach dir,$(INCLUDE_DIRS),-I./$(dir)) \
		-c $< -o $@
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-sh-build-c++-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)$(SH_CXX) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(SH_CXXFLAGS_$1) \
		$(foreach dir,$(INCLUDE_DIRS),-I./$(dir)) \
		-c $< -o $@
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-sh-build-library
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(SH_AR) rcs $@ $^
endef

define macro-sh-generate-install-header-rule
$(PREFIX)/$(SH_ARCH)/$(SH_ARCH)/include/$3/$2: $1/$2
	$(ECHO)[ "$(SILENT)" != 1 ] && set -x; \
	mkdir -p "$$(@D)"; \
	path=$$$$(cd "$$(@D)"; pwd); \
	printf -- "$(V_BEGIN_BLUE)$$$${path#$$(PREFIX)/$(SH_ARCH)/$(SH_ARCH)/}/$$(@F)$(V_END)\n";
	$(ECHO)$(INSTALL) -m 644 $$< $$@

install-$4: $4 $(PREFIX)/$(SH_ARCH)/$(SH_ARCH)/include/$3/$2
endef

define macro-sh-generate-install-lib-rule
$(PREFIX)/$(SH_ARCH)/$(SH_ARCH)/lib/$2: $1
	@printf -- "$(V_BEGIN_BLUE)lib/$2$(V_END)\n"
	mkdir -p "$$(@D)"
	$(ECHO)$(INSTALL) -m 644 $$< $$@

install-$3: $3 $(PREFIX)/$(SH_ARCH)/$(SH_ARCH)/lib/$2
endef

M68K_ARCH:= m68k-elf

M68K_AS:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-as$(EXE_EXT)
M68K_AR:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-ar$(EXE_EXT)
M68K_CC:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-gcc$(EXE_EXT)
M68K_CXX:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-g++$(EXE_EXT)
M68K_LD:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-gcc$(EXE_EXT)
M68K_NM:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-nm$(EXE_EXT)
M68K_OBJCOPY:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-objcopy$(EXE_EXT)
M68K_OBJDUMP:= $(INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-objdump$(EXE_EXT)

FIND:= find$(EXE_EXT)
INSTALL:= install$(EXE_EXT)
SED:= sed$(EXE_EXT)
STRIP:= strip$(EXE_EXT)

V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

ifeq ($(strip $(SILENT)),)
  ECHO=
else
  ECHO=@
endif
export ECHO

ifneq ($(strip $(NOCOLOR)),)
  V_BEGIN_BLACK=
  V_BEGIN_RED=
  V_BEGIN_GREEN=
  V_BEGIN_YELLOW=
  V_BEGIN_BLUE=
  V_BEGIN_MAGENTA=
  V_BEGIN_CYAN=
  V_BEGIN_WHITE=
  V_END=
endif

MAIN_TARGET= yaul
