PWD:= $(notdir $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

TARGET:= tga

ifndef BUILD_ROOT
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifndef INSTALL_ROOT
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifndef BUILD
  $(error Undefined BUILD (build directory))
endif

SUB_BUILD:= $(BUILD)/lib$(TARGET)

SRCS:= tga.c

CC_PREFIX:= sh-elf-
CXX:= $(CC_PREFIX)g++
AS:= $(CC_PREFIX)as
AO:= $(CC_PREFIX)as
CC:= $(CC_PREFIX)gcc
LD:= $(CC_PREFIX)gcc
NM:= $(CC_PREFIX)nm
OB:= $(CC_PREFIX)objcopy
OD:= $(CC_PREFIX)objdump
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
	-Wno-parentheses \
	-Wno-int-to-pointer-cast \
	-Wno-pointer-to-int-cast \
	-Wno-maybe-uninitialized \
	-Wno-pointer-sign
CCFLAGS_release:= -O2 $(CCFLAGS)
CCFLAGS_release-internal:= -O2 $(CCFLAGS) \
	-DRELEASE_INTERNAL
CCFLAGS_debug:= -O0 -g $(CCFLAGS) \
	-DDEBUG

SILENT?= @

OBJS:= $(SRCS:.c=.o)
DEPS:= $(SRCS:.c=.d)

OBJS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(OBJS))
OBJS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(OBJS))
OBJS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(OBJS))
DEPS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(DEPS))
DEPS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(DEPS))
DEPS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(DEPS))

define macro-build-object
	@printf -- "[1;33m$@[m\n"
	$(SILENT)$(CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(CCFLAGS_$1) \
		$(foreach dir,$(INCLUDES),-I./$(dir)) \
		-c $< -o $@
	$(SILENT)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-build-library
	@printf -- "[1;33m$@[m\n"
	$(SILENT)$(AR) rcs $@ $(OBJS_$1)
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
	$(SILENT)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a $(INSTALL_ROOT)/lib/

install-release-internal: release-internal
	$(SILENT)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a $(INSTALL_ROOT)/lib/

install-debug: debug
	$(SILENT)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a $(INSTALL_ROOT)/lib/

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
