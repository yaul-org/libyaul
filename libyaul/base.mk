# -*- mode: makefile -*-

SUB_BUILD:=$(BUILD)/lib$(TARGET)

LIB_FILE_base:= $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/lib$(TARGET).a

LIB_OBJS:= $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIB_SRCS)))
LIB_DEPS:= $(LIB_OBJS:.o=.d)

LIB_OBJS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS))
LIB_DEPS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_DEPS))

SUPPORT_OBJS:= $(patsubst %.c,%.o,$(patsubst %.cxx,%.o,$(patsubst %.S,%.o,$(SUPPORT_SRCS))))
SUPPORT_DEPS:= $(SUPPORT_OBJS:.o=.d)

SUPPORT_OBJS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(SUPPORT_OBJS))
SUPPORT_DEPS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(SUPPORT_DEPS))

LDSCRIPTS_all:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(LDSCRIPTS))
SPECS_all := $(addprefix $(THIS_ROOT)/lib$(TARGET)/,$(SPECS))
BOOTSTRAP_FILES_all = $(BOOTSTRAP_FILES)
USER_FILES_all = $(USER_FILES)

define macro-generate-install-file-rule
$(INSTALL_ROOT)/$3/$2: $1
	@printf -- "$(V_BEGIN_BLUE)$2$(V_END)\n"
	@mkdir -p $$(@D)
	$(ECHO)$(INSTALL) -m 644 $$< $$@

install-$4: $4 $(INSTALL_ROOT)/$3/$2
endef

.PHONY: all $(TYPE) install-$(TYPE)

.SUFFIXES:= .c .cxx .S .o .x

all: $(TYPE)

$(TYPE): $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) $(LIB_FILE_base) $(SUPPORT_OBJS_base) $(LDSCRIPTS_all) $(SPECS_all)

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE):
	$(ECHO)mkdir -p $@

$(LIB_FILE_base): $(LIB_OBJS_base)
	$(call macro-sh-build-library)

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.c
	$(call macro-sh-build-object,$(TYPE))

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.cxx
	$(call macro-sh-build-c++-object,$(TYPE))

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.S
	$(call macro-sh-build-object,$(TYPE))

$(BUILD_ROOT)/$(SUB_BUILD)/%.x: %.x
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)cat $< | awk '/^SEARCH_DIR[[:space:]]+(.+);$$/ { \
	    sub(/\$$INSTALL_ROOT/,"'$(INSTALL_ROOT)'/'$(SH_ARCH)'"); \
	} \
	{ print }' > $@

# Install header files
$(foreach TUPLE,$(INSTALL_HEADER_FILES), \
	$(eval P1= $(word 1,$(subst :, ,$(TUPLE)))) \
	$(eval P2= $(word 2,$(subst :, ,$(TUPLE)))) \
	$(eval P3= $(word 3,$(subst :, ,$(TUPLE)))) \
	$(eval $(call macro-sh-generate-install-header-rule,$(P1),$(P2),$(P3),$(TYPE))))

# Install support object files
$(foreach SUPPORT_OBJ,$(SUPPORT_OBJS_base), \
	$(eval $(call macro-sh-generate-install-lib-rule,$(SUPPORT_OBJ),$(notdir $(SUPPORT_OBJ)),$(TYPE))))

# Install files
$(foreach SPECS,$(SPECS_all), \
	$(eval $(call macro-generate-install-file-rule,$(SPECS),$(notdir $(SPECS)),$(SH_ARCH)/$(SH_ARCH)/lib,$(TYPE))))

$(foreach LDSCRIPT,$(LDSCRIPTS_all), \
	$(eval $(call macro-generate-install-file-rule,$(LDSCRIPT),$(notdir $(LDSCRIPT)),$(SH_ARCH)/$(SH_ARCH)/lib/ldscripts,$(TYPE))))

$(foreach BOOTSTRAP,$(BOOTSTRAP_FILES_all), \
	$(eval $(call macro-generate-install-file-rule,$(BOOTSTRAP),$(notdir $(BOOTSTRAP)),$(SH_ARCH)/share/$(TARGET)/bootstrap,$(TYPE))))

$(foreach USER_FILE,$(USER_FILES_all), \
	$(eval $(call macro-generate-install-file-rule,$(USER_FILE),$(notdir $(USER_FILE)),share,$(TYPE))))

# Install library
$(eval $(call macro-sh-generate-install-lib-rule,$(LIB_FILE_base),$(notdir $(LIB_FILE_base)),$(TYPE)))

clean:
	$(ECHO)if [ -d $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) ]; then \
		$(FIND) $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
	$(ECHO)$(RM) $(LIB_FILE_base)

-include $(SUPPORT_DEPS_base)
-include $(LIB_DEPS_base)
