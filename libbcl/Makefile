TARGET:= bcl

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

SUB_BUILD:= $(BUILD)/lib$(TARGET)

SRCS:= huffman.c \
	lz.c \
	prs.c \
	rice.c \
	rle.c \
	shannonfano.c

HEADER_FILES:= \
	bcl.h

ARCH?= sh-elf

CXX:= $(ARCH)-g++
AS:= $(ARCH)-as
AR:= $(ARCH)-ar
CC:= $(ARCH)-gcc
LD:= $(ARCH)-gcc
NM:= $(ARCH)-nm
OBJCOPY:= $(ARCH)-objcopy
OBJDUMP:= $(ARCH)-objdump
SED:= sed
INSTALL:= install

CCFLAGS:= -std=c99 \
	-s \
	-ffast-math \
	-fstrict-aliasing \
	-fomit-frame-pointer \
	-Wall \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses
CCFLAGS_release:= -O2 $(CCFLAGS)
CCFLAGS_release-internal:= -O2 $(CCFLAGS) \
	-DRELEASE_INTERNAL
CCFLAGS_debug:= -O0 -g $(CCFLAGS) \
	-DDEBUG

SILENT?= @
V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

OBJS:= $(SRCS:.c=.o)
DEPS:= $(SRCS:.c=.d)

OBJS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(OBJS))
OBJS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(OBJS))
OBJS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(OBJS))
DEPS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(DEPS))
DEPS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(DEPS))
DEPS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(DEPS))

HEADER_FILES_all:= $(addprefix $(BUILD_ROOT)/lib$(TARGET)/,$(HEADER_FILES))

define macro-build-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)$(CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(CCFLAGS_$1) \
		$(foreach dir,$(INCLUDES),-I./$(dir)) \
		-c $< -o $@
	$(SILENT)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-build-library
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)$(AR) rcs $@ $(OBJS_$1)
endef

define macro-install-files
	$(SILENT)$(RM) -r $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)
	$(SILENT)mkdir -p $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)
	$(SILENT)for header_file in $(HEADER_FILES_all); do \
	    install_header_file=$$(realpath -m $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)/$${header_file#$(BUILD_ROOT)/lib$(TARGET)/}); \
            printf -- "$(V_BEGIN_BLUE)$${header_file#$(BUILD_ROOT)/}$(V_END)\n"; \
            $(INSTALL) -m 644 $${header_file} $${install_header_file}; \
	done
endef

define macro-install-library
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$1/lib$(TARGET).a$(V_END)\n"
	$(SILENT)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/$1/lib$(TARGET).a $(INSTALL_ROOT)/$(ARCH)/lib/
endef

define macro-clean
	$(SILENT)$(RM) $(foreach DIR,$(BUILD_ROOT)/$(SUB_BUILD)/$1,\
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
