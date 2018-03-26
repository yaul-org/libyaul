# -*- mode: makefile -*-

LIB_FILE_base:= $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/lib$(TARGET).a

LIB_OBJS:= $(LIB_SRCS:.c=.o)
LIB_DEPS:= $(LIB_SRCS:.c=.d)

LIB_OBJS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_OBJS))
LIB_DEPS_base:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/,$(LIB_DEPS))

.PHONY: all $(TYPE) clean install-$(TYPE)

.SUFFIXES:= .c .o

all: $(TYPE)

$(TYPE): $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) $(LIB_FILE_base)

# Install header files
$(foreach TUPLE,$(INSTALL_HEADER_FILES), \
	$(eval P1= $(word 1,$(subst :, ,$(TUPLE)))) \
	$(eval P2= $(word 2,$(subst :, ,$(TUPLE)))) \
	$(eval P3= $(word 3,$(subst :, ,$(TUPLE)))) \
	$(eval $(call macro-sh-generate-install-header-rule,$(P1),$(P2),$(P3),$(TYPE))))

# Install library
$(eval $(call macro-sh-generate-install-lib-rule,$(LIB_FILE_base),$(notdir $(LIB_FILE_base)),$(TYPE)))

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE):
	mkdir -p $@

$(LIB_FILE_base): $(LIB_OBJS_base)
	$(call macro-sh-build-library)

$(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE)/%.o: %.c
	$(call macro-sh-build-object,$(TYPE))

clean:
	$(ECHO)if [ -d $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) ]; then \
		$(FIND) $(BUILD_ROOT)/$(SUB_BUILD)/$(TYPE) -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
	$(ECHO)$(RM) $(LIB_FILE_base)

-include $(LIB_DEPS_base)
