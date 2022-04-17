# -*- mode: makefile -*-

SUB_BUILD:=$(YAUL_BUILD)/lib$(TARGET)

LIB_FILE_base:= $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/lib$(TARGET).a

LIB_SRCS_C:= $(filter %.c,$(LIB_SRCS))
LIB_SRCS_S:= $(filter %.sx,$(LIB_SRCS))

LIB_OBJS_C:= $(patsubst %.c,%.o,$(LIB_SRCS_C))
LIB_OBJS_S:= $(patsubst %.sx,%.o,$(LIB_SRCS_S))

LIB_OBJS_C_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS_C))
LIB_OBJS_S_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS_S))

LIB_OBJS:= $(LIB_OBJS_C) $(LIB_OBJS_S)
LIB_DEPS:= $(LIB_OBJS:.o=.d)

LIB_OBJS_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS))
LIB_DEPS_base:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_DEPS))

.PHONY: all $(TYPE) clean install-$(TYPE) generate-cdb

.SUFFIXES:= .sx .c .o

all: $(TYPE)

$(TYPE): $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) $(LIB_FILE_base)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE):
	mkdir -p $@

$(LIB_FILE_base): $(LIB_OBJS_base)
	$(call macro-sh-build-library)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.c
	$(call macro-sh-build-object,$(TYPE))

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.sx
	$(call macro-sh-build-object,$(TYPE))

# Install header files
$(foreach TUPLE,$(INSTALL_HEADER_FILES), \
	$(eval P1= $(word 1,$(subst :, ,$(TUPLE)))) \
	$(eval P2= $(word 2,$(subst :, ,$(TUPLE)))) \
	$(eval P3= $(word 3,$(subst :, ,$(TUPLE)))) \
	$(eval $(call macro-sh-generate-install-header-rule,$(P1),$(P2),$(P3),$(TYPE))))

# Install library
$(eval $(call macro-sh-generate-install-lib-rule,$(LIB_FILE_base),$(notdir $(LIB_FILE_base)),$(TYPE)))

generate-cdb:
	$(ECHO)$(call macro-loop-update-cdb,$(LIB_OBJS_base),c,$(CDB_GCC),$(SH_CFLAGS_release),release,$(CDB_FILE))

clean:
	$(ECHO)if [ -d $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) ]; then \
		$(FIND) $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
	$(ECHO)$(RM) $(LIB_FILE_base)

-include $(LIB_DEPS_base)
