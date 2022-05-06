THIS_FILE:=$(firstword $(MAKEFILE_LIST))

$(shell mkdir -p $(SH_BUILD_DIR))

ifeq ($(strip $(SH_PROGRAM)),)
  $(error Empty SH_PROGRAM (SH program name))
endif

# $1 -> Path to asset file
# $2 -> Valid C symbol asset name
define macro-builtin-asset-rule
$(call macro-convert-build-path,$(addsuffix .o,$1)): $1
	@printf -- "$(V_BEGIN_CYAN)$(strip $1)$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/bin/bin2o $1 $2 $$@

SH_SRCS+= $(addsuffix .o,$1)
endef

$(foreach BUILTIN_ASSET,$(BUILTIN_ASSETS), \
	$(eval $(call macro-builtin-asset-rule,\
		$(call macro-word-split,$(BUILTIN_ASSET),1), \
		$(call macro-word-split,$(BUILTIN_ASSET),2))))

# Check that SH_SRCS don't include duplicates. Be mindful that sort remove
# duplicates
SH_SRCS_UNIQ= $(sort $(SH_SRCS))

SH_SRCS_C:= $(filter %.c,$(SH_SRCS_UNIQ))
SH_SRCS_CXX:= $(filter %.cxx,$(SH_SRCS_UNIQ)) \
	$(filter %.cpp,$(SH_SRCS_UNIQ)) \
	$(filter %.cc,$(SH_SRCS_UNIQ)) \
	$(filter %.C,$(SH_SRCS_UNIQ))
SH_SRCS_S:= $(filter %.sx,$(SH_SRCS_UNIQ))
SH_SRCS_OTHER:= $(filter-out %.c %.cxx %.cpp %.cc %.C %.sx,$(SH_SRCS_UNIQ))

SH_OBJS_UNIQ:= $(addsuffix .o,$(foreach SRC,$(SH_SRCS_C) $(SH_SRCS_CXX) $(SH_SRCS_S) $(SH_SRCS_OTHER),$(basename $(SRC))))
SH_OBJS_UNIQ:= $(foreach OBJ,$(SH_OBJS_UNIQ),$(call macro-convert-build-path,$(OBJ)))

SH_DEFSYMS=

ifneq ($(strip $(IP_MASTER_STACK_ADDR)),)
  SH_DEFSYMS+= -Wl,--defsym=__master_stack=$(IP_MASTER_STACK_ADDR)
endif

ifneq ($(strip $(IP_SLAVE_STACK_ADDR)),)
  SH_DEFSYMS+= -Wl,--defsym=__slave_stack=$(IP_SLAVE_STACK_ADDR)
endif

SH_LDFLAGS+= $(SH_DEFSYMS)
SH_LXXFLAGS+= $(SH_DEFSYMS)

ifeq ($(strip $(SH_SPECS)),)
  SH_SPECS:= yaul.specs yaul-main.specs
endif

# If there are any C++ files, add the specific C++ specs file. This is done to
# avoid adding (small) bloat to any C-only projects
ifneq ($(strip $(SH_SRCS_CXX)),)
  SH_CXX_SPECS:= yaul-main-c++.specs
endif

SH_DEPS:= $(SH_OBJS_UNIQ:.o=.d)
SH_TEMPS:= $(SH_OBJS_UNIQ:.o=.i) $(SH_OBJS_UNIQ:.o=.ii) $(SH_OBJS_UNIQ:.o=.s)

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
	$(ECHO)$(SH_CC) -MT $2 -MF $(addsuffix .d,$(basename $2)) -MD $(SH_CFLAGS) $(foreach specs,$(SH_SPECS),-specs=$(specs)) -c -o $2 $1
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
	$(ECHO)$(SH_CC) $(SH_CFLAGS) -c -o $2 $1
endef

# $1 -> $<
# $2 -> $@
define macro-generate-sh-build-c++-object
$2: $1
	@printf -- "$(V_BEGIN_YELLOW)$1$(V_END)\n"
	$(ECHO)$(SH_CXX) -MT $2 -MF $(addsuffix .d,$(basename $2)) -MD $(SH_CXXFLAGS) $(foreach specs,$(SH_SPECS),-specs=$(specs)) $(foreach specs,$(SH_CXX_SPECS),-specs=$(specs)) -c -o $2 $1
	$(ECHO)$(call macro-update-cdb,\
		$(CDB_CPP),\
		$(abspath $1),\
		$(abspath $2),\
		$(dir $(abspath $1)),\
		$(CDB_FILE),\
		$(SH_CXXFLAGS) $(SH_SYSTEM_INCLUDE_DIRS) $(SH_INCLUDE_DIRS))
endef

build: $(SH_PROGRAM).cue

$(SH_BUILD_PATH)/$(SH_PROGRAM).bin: $(SH_BUILD_PATH)/$(SH_PROGRAM).elf
	@printf -- "$(V_BEGIN_YELLOW)$(@F)$(V_END)\n"
	$(ECHO)$(SH_OBJCOPY) -O binary $< $@
	@[ -z "${SILENT}" ] && du -hs $@ | awk '{ print $$1; }' || true

$(SH_BUILD_PATH)/$(SH_PROGRAM).elf: $(SH_OBJS_UNIQ)
	@printf -- "$(V_BEGIN_YELLOW)$(@F)$(V_END)\n"
	$(ECHO)$(SH_LD) $(foreach specs,$(SH_SPECS),-specs=$(specs)) $(foreach specs,$(SH_CXX_SPECS),-specs=$(specs)) $(SH_OBJS_UNIQ) $(SH_LDFLAGS) $(foreach lib,$(SH_LIBRARIES),-l$(lib)) -o $@
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

$(SH_PROGRAM).iso: $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(SH_BUILD_PATH)/IP.BIN
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
    # This is a rather nasty hack to suppress any output from running the
    # pre/post-build-iso targets
	$(ECHO)$(MAKE) --no-print-directory $$([ -z "$(SILENT)" ] || printf -- "-s") -f $(THIS_FILE) pre-build-iso
	$(ECHO)mkdir -p $(IMAGE_DIRECTORY)
	$(ECHO)cp $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(IMAGE_DIRECTORY)/$(IMAGE_1ST_READ_BIN)
	$(ECHO)for txt in "ABS.TXT" "BIB.TXT" "CPY.TXT"; do \
	    if ! [ -s $(IMAGE_DIRECTORY)/$$txt ]; then \
		printf -- "empty\n" > $(IMAGE_DIRECTORY)/$$txt; \
	    fi \
	done
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-iso $(IMAGE_DIRECTORY) $(SH_BUILD_PATH)/IP.BIN $(SH_PROGRAM)
	$(ECHO)$(MAKE) --no-print-directory $$([ -z "$(SILENT)" ] || printf -- "-s") -f $(THIS_FILE) post-build-iso

$(SH_PROGRAM).ss: $(SH_BUILD_PATH)/$(SH_PROGRAM).bin $(SH_BUILD_PATH)/CART-IP.BIN
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)cat $(SH_BUILD_PATH)/CART-IP.BIN $(SH_BUILD_PATH)/$(SH_PROGRAM).bin > $@

$(SH_PROGRAM).cue: | $(SH_PROGRAM).iso
	@printf -- "$(V_BEGIN_YELLOW)$@$(V_END)\n"
	$(ECHO)$(YAUL_INSTALL_ROOT)/share/wrap-error $(YAUL_INSTALL_ROOT)/bin/make-cue "$(SH_PROGRAM).iso"

$(SH_BUILD_PATH)/IP.BIN: $(YAUL_INSTALL_ROOT)/share/yaul/ip/ip.sx $(SH_BUILD_PATH)/$(SH_PROGRAM).bin
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

$(SH_BUILD_PATH)/CART-IP.BIN: $(YAUL_INSTALL_ROOT)/share/yaul/ip/ip.sx $(SH_BUILD_PATH)/$(SH_PROGRAM).bin
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
	    $(SH_TEMPS) \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).asm \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).bin \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).elf \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).map \
	    $(SH_BUILD_PATH)/$(SH_PROGRAM).sym \
	    $(SH_BUILD_PATH)/IP.BIN \
	    $(SH_BUILD_PATH)/IP.BIN.map \
	    $(SH_BUILD_PATH)/CART-IP.BIN \
	    $(SH_BUILD_PATH)/CART-IP.BIN.map \
	    $(CDB_FILE)

list-targets:
	@$(MAKE) -pRrq -f $(THIS_FILE) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'

-include $(SH_DEPS)

undefine macro-update-cdb
undefine macro-generate-sh-build-object
undefine macro-generate-sh-build-asm-object
undefine macro-generate-sh-build-c++-object

undefine macro-builtin-asset-rule
