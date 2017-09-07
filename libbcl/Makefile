TARGET:= bcl

include ../env.mk

SUB_BUILD:= $(BUILD)/lib$(TARGET)

SRCS:= huffman.c \
	lz.c \
	prs.c \
	rice.c \
	rle.c \
	shannonfano.c

HEADER_FILES:= \
	bcl.h

SH_CFLAGS:= -std=c99 \
	-s \
	-ffast-math \
	-ffreestanding \
	-fstrict-aliasing \
	-fomit-frame-pointer \
	-Wall \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses
SH_CFLAGS_release:= -O2 $(SH_CFLAGS)
SH_CFLAGS_release-internal:= -O2 $(SH_CFLAGS) \
	-DRELEASE_INTERNAL
SH_CFLAGS_debug:= -O0 -g $(SH_CFLAGS) \
	-DDEBUG

OBJS:= $(SRCS:.c=.o)
DEPS:= $(SRCS:.c=.d)

OBJS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(OBJS))
OBJS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(OBJS))
OBJS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(OBJS))
DEPS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(DEPS))
DEPS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(DEPS))
DEPS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(DEPS))

HEADER_FILES_all:= $(addprefix $(THIS_ROOT)/lib$(TARGET)/,$(HEADER_FILES))

define macro-build-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(SH_CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(SH_CFLAGS_$1) \
		$(foreach dir,$(INCLUDES),-I./$(dir)) \
		-c $< -o $@
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-build-library
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(SH_AR) rcs $@ $(OBJS_$1)
endef

define macro-install-files
	$(ECHO)$(RM) -r $(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)
	$(ECHO)mkdir -p $(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)
	$(ECHO)for header_file in $(HEADER_FILES_all); do \
	    path="$(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)/$${header_file#$(THIS_ROOT)/lib$(TARGET)/}"; \
	    install_header_file=$$(python -c "import os; print os.path.abspath(\"$${path}\")"); \
	    printf -- "$(V_BEGIN_BLUE)$${header_file#$(THIS_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $${header_file} $${install_header_file}; \
	done
endef

define macro-install-library
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$1/lib$(TARGET).a$(V_END)\n"
	$(ECHO)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/$1/lib$(TARGET).a $(INSTALL_ROOT)/sh-elf/sh-elf/lib/
endef

define macro-clean
	$(ECHO)$(RM) $(foreach DIR,$(BUILD_ROOT)/$(SUB_BUILD)/$1,\
		$(DIR)/*.o \
		$(DIR)/*.d) \
		$(BUILD_ROOT)/$(SUB_BUILD)/$1/lib$(TARGET).a
endef

.PHONY: all release release-internal debug clean clean-release clean-release-internal clean-debug

.SUFFIXES:= .c .S .o

all: release release-internal debug

release: $(BUILD_ROOT)/$(SUB_BUILD)/release $(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a
release-internal: $(BUILD_ROOT)/$(SUB_BUILD)/release-internal $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a
debug: $(BUILD_ROOT)/$(SUB_BUILD)/debug $(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a

install-release: release
	$(call macro-install-files)
	$(call macro-install-library,release)

install-release-internal: release-internal
	$(call macro-install-files)
	$(call macro-install-library,release-internal)

install-debug: debug
	$(call macro-install-files)
	$(call macro-install-library,debug)

$(BUILD_ROOT)/$(SUB_BUILD)/release:
	mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal:
	mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/debug:
	mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a: $(OBJS_release)
	$(call macro-build-library,release)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a: $(OBJS_release-internal)
	$(call macro-build-library,release-internal)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a: $(OBJS_debug)
	$(call macro-build-library,debug)

$(BUILD_ROOT)/$(SUB_BUILD)/release/%.o: %.c
	$(call macro-build-object,release,$@,$*)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/%.o: %.c
	$(call macro-build-object,release-internal,$@,$*)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/%.o: %.c
	$(call macro-build-object,debug,$@,$*)

clean: clean-release clean-release-internal clean-debug

clean-release:
	$(call macro-clean,release)

clean-release-internal:
	$(call macro-clean,release-internal)

clean-debug:
	$(call macro-clean,debug)

-include $(DEPS_release)
-include $(DEPS_debug)
-include $(DEPS_release-internal)
