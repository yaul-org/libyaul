include ../../env.mk

TARGET:= ssload
PROGRAM:= $(TARGET)$(EXE_EXT)

SUB_BUILD:=$(BUILD)/tools/$(TARGET)

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
LIB_DIRS:= /usr/lib
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

ifneq ($(strip $(DEBUG)),)
CFLAGS+= -DDEBUG
endif

OBJS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

$(BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM): $(BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
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
	$(ECHO)$(RM) $(OBJS) $(DEPS) $(BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

distclean: clean

install: $(BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$(PROGRAM)$(V_END)\n"
	$(ECHO)mkdir -p $(INSTALL_ROOT)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/

-include $(DEPS)
