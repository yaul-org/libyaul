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

ifeq '$(OS)' "Windows_NT"
EXE_EXT:= .exe
endif

SH_AS:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-as$(EXE_EXT)
SH_AR:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-ar$(EXE_EXT)
SH_CC:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc$(EXE_EXT)
SH_CXX:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-g++$(EXE_EXT)
SH_LD:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc$(EXE_EXT)
SH_NM:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-nm$(EXE_EXT)
SH_OBJCOPY:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objcopy$(EXE_EXT)
SH_OBJDUMP:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objdump$(EXE_EXT)

M68K_AS:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-as$(EXE_EXT)
M68K_AR:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-ar$(EXE_EXT)
M68K_CC:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc$(EXE_EXT)
M68K_CXX:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-g++$(EXE_EXT)
M68K_LD:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc$(EXE_EXT)
M68K_NM:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-nm$(EXE_EXT)
M68K_OBJCOPY:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objcopy$(EXE_EXT)
M68K_OBJDUMP:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objdump$(EXE_EXT)

FIND:= find
INSTALL:= install
SED:= sed
STRIP:= strip

V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

ifeq ($(strip $(SILENT)),1)
  ECHO=@
else
  ECHO=
endif
export ECHO

MAIN_TARGET= yaul
