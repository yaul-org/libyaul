TARGET:= ssload

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

ifeq ($(strip $(SILENT)),1)
  ECHO=@
else
  ECHO=
endif
export ECHO

SUB_BUILD:=$(BUILD)/tools/$(TARGET)

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
STRIP:= strip

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
LDFLAGS:=

SRCS:= ssload.c \
	console.c \
	datalink.c \
	drivers.c \
	shared.c \
	usb-cartridge.c
INCLUDES:=
LIB_DIRS:=
LIBS:=

ifneq ($(strip $(HAVE_LIBFTD2XX)),)
CFLAGS+= -DHAVE_LIBFTD2XX=1
INCLUDES+= ./libftd2xx/release
LIB_DIRS+= ./libftd2xx/release/build/$(shell uname -m)
LIBS+= ftd2xx dl pthread rt
else
CFLAGS+= $(shell pkg-config --cflags libftdi)
LDFLAGS+= $(shell pkg-config --libs libftdi)
endif

OBJS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)

$(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET): $(BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(CC) -o $@ $(OBJS) \
		$(foreach DIR,$(LIB_DIRS),-L$(DIR)) \
		$(foreach LIB,$(LIBS),-l$(LIB)) \
		$(LDFLAGS)
	$(ECHO)$(STRIP) -s $@

$(BUILD_ROOT)/$(SUB_BUILD):
	$(ECHO)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/%.o: %.c
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)$(CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$*.d $(CFLAGS) \
		$(foreach DIR,$(INCLUDES),-I$(DIR)) \
		-c -o $@ $<
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$*.d

clean:
	$(ECHO)$(RM) $(OBJS) $(DEPS) $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)

distclean: clean

install: $(BUILD_ROOT)/$(SUB_BUILD)/$(TARGET)
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$(TARGET)$(V_END)\n"
	$(ECHO)mkdir -p $(INSTALL_ROOT)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/

-include $(DEPS)
