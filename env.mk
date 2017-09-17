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

SH_AS:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-as
SH_AR:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-ar
SH_CC:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_CXX:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-g++
SH_LD:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_NM:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-nm
SH_OBJCOPY:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objcopy
SH_OBJDUMP:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objdump

M68K_AS:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-as
M68K_AR:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-ar
M68K_CC:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_CXX:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-g++
M68K_LD:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_NM:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-nm
M68K_OBJCOPY:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objcopy
M68K_OBJDUMP:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objdump

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
