include ../../env.mk

TARGET:= gdb-ftdi-proxy
PROGRAM:= $(TARGET)$(EXE_EXT)

SUB_BUILD:=$(YAUL_BUILD)/tools/$(TARGET)

CFLAGS:= -s \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses
LDFLAGS:=

SRCS:= gdb-ftdi-proxy.c
INCLUDES:=
LIB_DIRS:= /usr/lib
LIBS:=

ifneq ($(strip $(HAVE_LIBFTD2XX)),)
CFLAGS+= -DHAVE_LIBFTD2XX=1
INCLUDES+= ./libftd2xx/release
LIB_DIRS+= ./libftd2xx/release/build/$(shell uname -m)
LIBS+= ftd2xx dl pthread rt
else
ifneq ($(strip $(HAVE_LIBFTDI1_32BIT)),)
INCLUDES+= libftdi1-1.4/include/libftdi1 libusb-1.0/include/libusb-1.0
LIB_DIRS+= libftdi1-1.4/lib libusb-1.0/lib
LIBS+= ftdi1 usb-1.0
else
CFLAGS+= $(shell pkg-config --cflags libftdi1)
LDFLAGS+= $(shell pkg-config --libs libftdi1)
endif
endif

ifeq ($(strip $(DEBUG)),)
CFLAGS+= -O2
else
CFLAGS+= -DDEBUG -ggdb3 -O0
endif

OBJS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM): $(YAUL_BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(YAUL_BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(CC) -o $@ $(OBJS) \
		$(foreach DIR,$(LIB_DIRS),-L$(DIR)) \
		$(foreach LIB,$(LIBS),-l$(LIB)) \
		$(LDFLAGS)
	$(ECHO)if [ -z "${DEBUG}" ]; then \
	    $(STRIP) -s $@; \
	fi

$(YAUL_BUILD_ROOT)/$(SUB_BUILD):
	$(ECHO)mkdir -p $@

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/%.o: %.c
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(YAUL_BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)$(CC) -Wp,-MMD,$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$*.d $(CFLAGS) \
		$(foreach DIR,$(INCLUDES),-I$(DIR)) \
		-c -o $@ $<
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$*.d

clean:
	$(ECHO)$(RM) $(OBJS) $(DEPS) $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

distclean: clean

install: $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$(PROGRAM)$(V_END)\n"
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(YAUL_INSTALL_ROOT)/bin/

-include $(DEPS)
