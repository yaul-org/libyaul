THIS_FILE:=$(firstword $(MAKEFILE_LIST))

$(shell mkdir -p $(SH_BUILD_DIR))

ifeq ($(strip $(SH_PROGRAM)),)
  $(error Empty SH_PROGRAM (SH program name))
endif

# Each relative or absolute path will be converted to using '@' instead of '/'
define macro-convert-build-path
$(SH_BUILD_PATH)/$(subst /,@,$(abspath $1))
endef

define macro-word-split
$(word $2,$(subst ;, ,$1))
endef

# Check that SH_SRCS and SH_SRCS_NO_LINK don't include duplicates. Be mindful
# that sort remove duplicates
SH_SRCS_UNIQ= $(sort $(SH_SRCS))
SH_SRCS_NO_LINK_UNIQ= $(sort $(SH_SRCS_NO_LINK))

SH_SRCS_C:= $(filter %.c,$(SH_SRCS_UNIQ))
SH_SRCS_CXX:= $(filter %.cxx,$(SH_SRCS_UNIQ)) \
	$(filter %.cpp,$(SH_SRCS_UNIQ)) \
	$(filter %.cc,$(SH_SRCS_UNIQ)) \
	$(filter %.C,$(SH_SRCS_UNIQ))
SH_SRCS_S:= $(filter %.sx,$(SH_SRCS_UNIQ))
SH_SRCS_ROMDISK:= $(addsuffix .romdisk,$(ROMDISK_DIRS))

SH_SRCS_NO_LINK_C:= $(filter %.c,$(SH_SRCS_NO_LINK_UNIQ))

SH_OBJS_UNIQ:= $(addsuffix .o,$(foreach SRC,$(SH_SRCS_C) $(SH_SRCS_CXX) $(SH_SRCS_S),$(basename $(SRC)))) \
	$(addsuffix .o,$(SH_SRCS_ROMDISK))
SH_OBJS_UNIQ:= $(foreach OBJ,$(SH_OBJS_UNIQ),$(call macro-convert-build-path,$(OBJ)))

SH_OBJS_NO_LINK_UNIQ:= $(addsuffix .o,$(foreach dir,$(SH_SRCS_NO_LINK_C),$(basename $(dir))))
SH_OBJS_NO_LINK_UNIQ:= $(foreach OBJ,$(SH_OBJS_NO_LINK_UNIQ),$(call macro-convert-build-path,$(OBJ)))

ifeq ($(strip $(SH_OBJS_UNIQ)),)
  # If both SH_OBJS_UNIQ and SH_OBJS_NO_LINK_UNIQ is empty
  ifeq ($(strip $(SH_OBJS_NO_LINK_UNIQ)),)
    $(error Empty SH_OBJS (SH source list))
  endif
endif

ifneq ($(strip $(ROMDISK_DIRS)),)
  ifeq ($(strip $(ROMDISK_SYMBOLS)),)
    $(error Empty ROMDISK_SYMBOLS (symbols for ROMDISK))
  endif
endif

ifneq ($(strip $(ROMDISK_SYMBOLS)),)
  ifeq ($(strip $(ROMDISK_SYMBOLS)),)
    $(error Empty ROMDISK_DIRS (paths for ROMDISK))
  endif
endif

SH_DEFSYMS=

ifneq ($(strip $(IP_MASTER_STACK_ADDR)),)
  SH_DEFSYMS+= -Wl,--defsym=__master_stack=$(IP_MASTER_STACK_ADDR)
endif

ifneq ($(strip $(IP_SLAVE_STACK_ADDR)),)
  SH_DEFSYMS+= -Wl,--defsym=__slave_stack=$(IP_SLAVE_STACK_ADDR)
endif

SH_LDFLAGS+= $(SH_DEFSYMS)
SH_LXXFLAGS+= $(SH_DEFSYMS)

SH_SPECS= yaul.specs

ifeq ($(strip $(SH_CUSTOM_SPECS)),)
  SH_SPECS+= yaul-main.specs
else
  SH_SPECS+= $(SH_CUSTOM_SPECS)
endif

SH_DEPS:= $(SH_OBJS_UNIQ:.o=.d)
SH_TEMPS:= $(SH_OBJS_UNIQ:.o=.i) $(SH_OBJS_UNIQ:.o=.ii) $(SH_OBJS_UNIQ:.o=.s)
SH_DEPS_NO_LINK:= $(SH_OBJS_NO_LINK_UNIQ:.o=.d)

# Parse out included paths from GCC when the specs files are used. This is used
# to explictly populate each command database entry with include paths
SH_INCLUDE_DIRS:=$(shell echo | $(SH_CC) -E -Wp,-v -nostdinc $(foreach specs,$(SH_SPECS),-specs=$(specs)) - 2>&1 | \
	awk '/^\s/ { sub(/^\s+/,"-I"); print }')
SH_SYSTEM_INCLUDE_DIRS=$(shell echo | $(SH_CC) -E -Wp,-v - 2>&1 | \
	awk '/^\s/ { sub(/^\s+/,"-isystem "); print }')

ifeq ($(strip $(YAUL_CDB)),1)
# $1 -> Absolute path to compiler executable
# $2 -> Absolute path to input file
# $3 -> Absolute path to output file
# $4 -> Absolute build path
# $5 -> Absolute path to output compile DB file
# $6 -> Compiler flags
define macro-update-cdb
  $(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/share/update-cdb $1 "$2" "$3" $4 $5 $6
endef
else
define macro-update-cdb
endef
endif

# $1 -> $<
# $2 -> $@
define macro-generate-sh-build-object
$2: $1
	@printf -- "$(V_BEGIN_YELLOW)$1$(V_END)\n"
	$(ECHO)$(SH_CC) -MF $(addsuffix .d,$(basename $2)) -MD $(SH_CFLAGS) $(foreach specs,$(SH_SPECS),-specs=$(specs)) -c -o $2 $1
	$(ECHO)$(call macro-update-cdb,\
		$(CDB_GCC),\
		$(abspath $1),\
		$(abspath $2),\
		$(dir $(abspath $1)),\
		$(CDB_FILE),\
		$(SH_CFLAGS) $(SH_SYSTEM_INCLUDE_DIRS) $(SH_INCLUDE_DIRS))
endef

# $1 -> $<
# $2 -> $@
define macro-generate-sh-build-asm-object
$2: $1
	@printf -- "$(V_BEGIN_YELLOW)$1$(V_END)\n"
	$(ECHO)$(SH_AS) $(SH_AFLAGS) -o $2 $1
endef

# $1 -> $<
# $2 -> $@
define macro-generate-sh-build-c++-object
$2: $1
	@printf -- "$(V_BEGIN_YELLOW)$1$(V_END)\n"
	$(ECHO)$(SH_CXX) -MF $(addsuffix .d,$(basename $2)) -MD $(SH_CXXFLAGS) $(foreach specs,$(SH_SPECS),-specs=$(specs)) -c -o $2 $1
	$(ECHO)$(call macro-update-cdb,\
		$(CDB_CPP),\
		$(abspath $1),\
		$(abspath $2),\
		$(dir $(abspath $1)),\
		$(CDB_FILE),\
		$(SH_CXXFLAGS) $(SH_SYSTEM_INCLUDE_DIRS) $(SH_INCLUDE_DIRS))
endef

# $1 -> Symbol
# $2 -> Absolute filename build path
# $3 -> Directory
define macro-generate-romdisk-rule
$2.d:
# When generating the dependency list file, use absolute paths to avoid conflict
# between existing targets. For example, if the ROMDISK directory is named
# "romdisk", find(1) will print out "romdisk" in its output. If any of the C
# source files is also named romdisk.c, or the target is "romdisk", then it will
# cause a conflict
	$(ECHO)find $(abspath $3) -type f,d -print 2>/dev/null | \
		awk 'BEGIN { print "$2: \\"; } \
		     { print "\t" $$$$0 " \\"; } \
		     END { print "\t" $$$$0; }' > $$@

$2: $2.d
	@printf -- "$(V_BEGIN_YELLOW)$(strip $1).romdisk$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/genromfs$(EXE_EXT) $(ROMDISK_FLAGS) -d $3 -f $2

$2.o: $2
	@printf -- "$(V_BEGIN_YELLOW)$(strip $1).romdisk.o$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/bin/bin2o $$< "$(strip $1)_romdisk" $$@

-include "$2.d"
endef

build: $(SH_PROGRAM).cue

$(SH_BUILD_PATH)/$(SH_PROGRAM).bin: $(SH_BUILD_PATH)/$(SH_PROGRAM).elf
	@printf -- "$(V_BEGIN_YELLOW)$(@F)$(V_END)\n"
	$(ECHO)$(SH_OBJCOPY) -O binary $< $@
	@[ -z "${SILENT}" ] && du -hs $@ | awk '{ print $$1; }' || true

$(SH_BUILD_PATH)/$(SH_PROGRAM).elf: $(SH_OBJS_UNIQ) $(SH_OBJS_NO_LINK_UNIQ)
	@printf -- "$(V_BEGIN_YELLOW)$(@F)$(V_END)\n"
	$(ECHO)$(SH_LD) $(foreach specs,$(SH_SPECS),-specs=$(specs)) $(SH_OBJS_UNIQ) $(SH_LDFLAGS) $(foreach lib,$(SH_LIBRARIES),-l$(lib)) -o $@
	$(ECHO)$(SH_NM) $(SH_BUILD_PATH)/$(SH_PROGRAM).elf > $(SH_BUILD_PATH)/$(SH_PROGRAM).sym
	$(ECHO)$(SH_OBJDUMP) -S $(SH_BUILD_PATH)/$(SH_PROGRAM).elf > $(SH_BUILD_PATH)/$(SH_PROGRAM).asm

$(foreach SRC,$(SH_SRCS_C), \
	$(eval $(call macro-generate-sh-build-object,$(SRC),\
		$(call macro-convert-build-path,$(addsuffix .o,$(basename $(SRC)))))))

$(foreach SRC,$(SH_SRCS_CXX), \
	$(eval $(call macro-generate-sh-build-c++-object,$(SRC),\
		$(call macro-convert-build-path,$(addsuffix .o,$(basename $(SRC)))))))

$(foreach SRC,$(SH_SRCS_S), \
	$(eval $(call macro-generate-sh-build-asm-object,$(SRC),\
		$(call macro-convert-build-path,$(addsuffix .o,$(basename $(SRC)))))))

# Each entry is <symbol>;<directory>
$(foreach SYMBOL_DIR,$(join $(ROMDISK_SYMBOLS),$(addprefix ;,$(SH_SRCS_ROMDISK))), \
	$(eval $(call macro-generate-romdisk-rule,\
		$(call macro-word-split,$(SYMBOL_DIR),1),\
		$(call macro-convert-build-path,$(call macro-word-split,$(SYMBOL_DIR),2)),\
		$(basename $(call macro-word-split,$(SYMBOL_DIR),2)))))

$(SH_PROGRAM).iso: $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(SH_BUILD_PATH)/IP.BIN
	$(ECHO)$(MAKE) --no-print-directory -f $(THIS_FILE) pre-build-iso
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)mkdir -p $(IMAGE_DIRECTORY)
	$(ECHO)cp $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(IMAGE_DIRECTORY)/$(IMAGE_1ST_READ_BIN)
	$(ECHO)for txt in "ABS.TXT" "BIB.TXT" "CPY.TXT"; do \
	    if ! [ -s $(IMAGE_DIRECTORY)/$$txt ]; then \
		printf -- "empty\n" > $(IMAGE_DIRECTORY)/$$txt; \
	    fi \
	done
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-iso $(IMAGE_DIRECTORY) $(SH_BUILD_PATH)/IP.BIN $(SH_PROGRAM)
	$(ECHO)$(MAKE) --no-print-directory -f $(THIS_FILE) post-build-iso

$(SH_PROGRAM).ss: $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(SH_BUILD_PATH)/CART-IP.BIN
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)cat $(SH_BUILD_PATH)/CART-IP.BIN $(SH_BUILD_PATH)/$(SH_PROGRAM).bin > $@

$(SH_PROGRAM).cue: $(SH_PROGRAM).iso
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-cue "$(SH_PROGRAM).iso"

$(SH_BUILD_PATH)/IP.BIN: $(YAUL_INSTALL_ROOT)/share/yaul/bootstrap/ip.sx $(SH_BUILD_PATH)/$(SH_PROGRAM).bin
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-ip \
	    "$(SH_BUILD_PATH)/$(SH_PROGRAM).bin" \
		"$(IP_VERSION)" \
		$(IP_RELEASE_DATE) \
		"$(IP_AREAS)" \
		"$(IP_PERIPHERALS)" \
		'"$(IP_TITLE)"' \
		$(IP_MASTER_STACK_ADDR) \
		$(IP_SLAVE_STACK_ADDR) \
		$(IP_1ST_READ_ADDR) \
	    $(IP_1ST_READ_SIZE)

$(SH_BUILD_PATH)/CART-IP.BIN: $(YAUL_INSTALL_ROOT)/share/yaul/bootstrap/ip.sx $(SH_BUILD_PATH)/$(SH_PROGRAM).bin
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-ip \
	    "$(SH_BUILD_PATH)/$(SH_PROGRAM).bin" \
		"$(IP_VERSION)" \
		$(IP_RELEASE_DATE) \
		"$(IP_AREAS)" \
		"$(IP_PERIPHERALS)" \
		'"$(IP_TITLE)"' \
		$(IP_MASTER_STACK_ADDR) \
		$(IP_SLAVE_STACK_ADDR) \
		$(IP_1ST_READ_ADDR) \
	    -1 && mv -f $(SH_BUILD_PATH)/IP.BIN $(SH_BUILD_PATH)/CART-IP.BIN

clean:
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$(SH_PROGRAM)$(V_END) $(V_BEGIN_GREEN)clean$(V_END)\n"
	$(ECHO)-rm -f \
	    $(SH_PROGRAM).cue \
	    $(SH_PROGRAM).iso \
	    $(SH_PROGRAM).ss \
	    $(SH_OBJS_UNIQ) \
	    $(SH_DEPS) \
	    $(SH_DEPS_NO_LINK) \
	    $(SH_TEMPS) \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).asm \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).bin \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).elf \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).map \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).sym \
	    $(SH_OBJS_NO_LINK_UNIQ) \
	    $(SH_DEPS_NO_LINK) \
	    $(SH_BUILD_PATH)/IP.BIN \
	    $(SH_BUILD_PATH)/IP.BIN.map \
	    $(SH_BUILD_PATH)/CART-IP.BIN \
	    $(SH_BUILD_PATH)/CART-IP.BIN.map \
	    $(CDB_FILE) \
	    $(foreach DIR,$(SH_SRCS_ROMDISK),$(call macro-convert-build-path,$(DIR)))

list-targets:
	@$(MAKE) -pRrq -f $(THIS_FILE) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'

-include $(SH_DEPS)
-include $(SH_DEPS_NO_LINK)
