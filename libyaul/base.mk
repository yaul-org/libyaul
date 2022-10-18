# -*- mode: makefile -*-

SUB_BUILD:=$(YAUL_BUILD)/lib$(TARGET)

LIB_FILE_base:= $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/lib$(TARGET).a

LIB_SRCS_C:= $(filter %.c,$(LIB_SRCS))
LIB_SRCS_CXX:= $(filter %.cxx,$(LIB_SRCS))
LIB_SRCS_S:= $(filter %.sx,$(LIB_SRCS))

LIB_OBJS_C:= $(patsubst %.c,%.o,$(LIB_SRCS_C))
LIB_OBJS_CXX:= $(patsubst %.cxx,%.o,$(LIB_SRCS_CXX))
LIB_OBJS_S:= $(patsubst %.sx,%.o,$(LIB_SRCS_S))

LIB_OBJS_C_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS_C))
LIB_OBJS_CXX_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS_CXX))
LIB_OBJS_S_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS_S))

LIB_OBJS:= $(LIB_OBJS_C) $(LIB_OBJS_CXX) $(LIB_OBJS_S)
LIB_DEPS:= $(LIB_OBJS:.o=.d)

LIB_OBJS_base:= $(LIB_OBJS_C_base) $(LIB_OBJS_CXX_base) $(LIB_OBJS_S_base)
LIB_DEPS_base:= $(LIB_OBJS_base:.o=.d)

SUPPORT_SRCS_C:= $(filter %.c,$(SUPPORT_SRCS))
SUPPORT_SRCS_CXX:= $(filter %.cxx,$(SUPPORT_SRCS))
SUPPORT_SRCS_S:= $(filter %.sx,$(SUPPORT_SRCS))

SUPPORT_OBJS_C:= $(patsubst %.c,%.o,$(SUPPORT_SRCS_C))
SUPPORT_OBJS_CXX:= $(patsubst %.cxx,%.o,$(SUPPORT_SRCS_CXX))
SUPPORT_OBJS_S:= $(patsubst %.sx,%.o,$(SUPPORT_SRCS_S))

SUPPORT_OBJS_C_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(SUPPORT_OBJS_C))
SUPPORT_OBJS_CXX_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(SUPPORT_OBJS_CXX))
SUPPORT_OBJS_S_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(SUPPORT_OBJS_S))

SUPPORT_OBJS:= $(SUPPORT_OBJS_C) $(SUPPORT_OBJS_CXX) $(SUPPORT_OBJS_S)
SUPPORT_DEPS:= $(SUPPORT_OBJS:.o=.d)

SUPPORT_OBJS_base:= $(SUPPORT_OBJS_C_base) $(SUPPORT_OBJS_CXX_base) $(SUPPORT_OBJS_S_base)
SUPPORT_DEPS_base:= $(SUPPORT_OBJS_base:.o=.d)

LDSCRIPTS_all:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(LDSCRIPTS))
SPECS_all := $(addprefix $(THIS_ROOT)/lib$(TARGET)/,$(SPECS))
IP_FILES_all = $(IP_FILES)
USER_FILES_all = $(USER_FILES)
HELPER_FILES_all = $(HELPER_FILES)

# $1 ->
# $2 ->
# $3 ->
# $4 ->
# $5 ->
define macro-generate-install-file-rule
$(YAUL_PREFIX)/$3/$2: $1
	@printf -- "$(V_BEGIN_BLUE)$2$(V_END)\n"
	@mkdir -p $$(@D)
	$(ECHO)$(INSTALL) -m $5 $$< $$@

install-$4: $4 $(YAUL_PREFIX)/$3/$2
endef

.PHONY: all $(TYPE) clean install-$(TYPE) generate-cdb

.SUFFIXES:= .c .cxx .sx .o .x

all: $(TYPE)

$(TYPE): $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) $(LIB_FILE_base) $(SUPPORT_OBJS_base) $(LDSCRIPTS_all) $(SPECS_all)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE):
	$(ECHO)mkdir -p $@

$(LIB_FILE_base): $(LIB_OBJS_base)
	$(call macro-sh-build-library)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.c
	$(call macro-sh-build-object,$(TYPE))

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.cxx
	$(call macro-sh-build-c++-object,$(TYPE))

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.sx
	$(call macro-sh-build-object,$(TYPE))

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/%.x: %.x
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(YAUL_BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)cat $< | awk '/^SEARCH_DIR[[:space:]]+(.+);$$/ { \
	    sub(/\$$INSTALL_ROOT/,"'$(YAUL_PREFIX)'/'$(YAUL_ARCH_SH_PREFIX)'"); \
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
	$(eval $(call macro-generate-install-file-rule,$(SPECS),$(notdir $(SPECS)),$(YAUL_ARCH_SH_PREFIX)/lib,$(TYPE),644)))

$(foreach LDSCRIPT,$(LDSCRIPTS_all), \
	$(eval $(call macro-generate-install-file-rule,$(LDSCRIPT),$(notdir $(LDSCRIPT)),$(YAUL_ARCH_SH_PREFIX)/lib/ldscripts,$(TYPE),644)))

$(foreach IP,$(IP_FILES_all), \
	$(eval $(call macro-generate-install-file-rule,$(IP),$(IP),share/$(TARGET),$(TYPE),644)))

$(foreach USER_FILE,$(USER_FILES_all), \
	$(eval $(call macro-generate-install-file-rule,$(USER_FILE),$(notdir $(USER_FILE)),share,$(TYPE),644)))

$(foreach HELPER_FILE,$(HELPER_FILES_all), \
	$(eval $(call macro-generate-install-file-rule,$(HELPER_FILE),$(notdir $(HELPER_FILE)),share,$(TYPE),755)))

# Install library
$(eval $(call macro-sh-generate-install-lib-rule,$(LIB_FILE_base),$(notdir $(LIB_FILE_base)),$(TYPE)))

generate-cdb:
	$(ECHO)$(call macro-loop-update-cdb,$(LIB_OBJS_C_base),c,$(CDB_GCC),$(SH_CFLAGS_release),release,$(CDB_FILE))
	$(ECHO)$(call macro-loop-update-cdb,$(SUPPORT_OBJS_C_base),c,$(CDB_GCC),$(SH_CFLAGS_release),release,$(CDB_FILE))
	$(ECHO)$(call macro-loop-update-cdb,$(SUPPORT_OBJS_CXX_base),cxx,$(CDB_CPP),$(SH_CXXFLAGS_release),release,$(CDB_FILE))

clean:
	$(ECHO)if [ -d $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) ]; then \
		$(FIND) $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
	$(ECHO)$(RM) $(LIB_FILE_base)
	$(ECHO)$(RM) $(CDB_FILE)

-include $(SUPPORT_DEPS_base)
-include $(LIB_DEPS_base)
