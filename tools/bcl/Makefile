TARGET:= bcl

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

SUB_BUILD:=$(BUILD)/tools/$(TARGET)

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

INSTALL:= install
SED:= sed

CFLAGS:= -O2 \
	-s \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses

SRCS:= bcl.c

INCLUDES:= ./libftd2xx/release

OBJS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)

$(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET): $(BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)$(CC) -o $@ $(OBJS)

$(BUILD_ROOT)/$(SUB_BUILD):
	$(SILENT)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/%.o: %.c
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)mkdir -p $(@D)
	$(SILENT)$(CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$*.d $(CFLAGS) \
		$(foreach DIR,$(INCLUDES),-I$(DIR)) \
		-c -o $@ $<
	$(SILENT)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$*.d

clean:
	$(SILENT)$(RM) $(OBJS) $(DEPS) $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)

distclean: clean

install: $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$(TARGET)$(V_END)\n"
	$(SILENT)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/
	@printf -- "$(V_BEGIN_BLUE)fsck.genromfs$(V_END)\n"
	$(SILENT)$(INSTALL) -m 755 fsck.genromfs $(INSTALL_ROOT)/bin/

-include $(DEPS)
