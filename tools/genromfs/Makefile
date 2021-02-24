include ../../env.mk

TARGET:= genromfs
PROGRAM:= $(TARGET)$(EXE_EXT)

# This is a special flag for cross compiling Pacman packages
ifneq ($(strip $(BUILD_CROSS)),)
CC:= x86_64-w64-mingw32-gcc
STRIP:= x86_64-w64-mingw32-strip
LDFLAGS:= -lshlwapi -lws2_32

PROGRAM:= $(TARGET).exe
endif

SUB_BUILD:=$(YAUL_BUILD)/tools/$(TARGET)

CFLAGS:= -O2 \
	-s \
	-Wall \
	-Wextra \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses \
	-Wno-sign-compare

LDFLAGS?=

ifeq ($(OS), Windows_NT)
LDFLAGS+= -lshlwapi -lws2_32
endif

SRCS:= genromfs.c
INCLUDES:=

OBJS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM): $(YAUL_BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(YAUL_BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(CC) -o $@ $(OBJS) $(LDFLAGS)
	$(ECHO)$(STRIP) -s $@

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
	$(ECHO)mkdir -p $(YAUL_PREFIX)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(YAUL_PREFIX)/bin/
	@printf -- "$(V_BEGIN_BLUE)fsck.genromfs$(V_END)\n"
	$(ECHO)$(INSTALL) -m 755 fsck.genromfs $(YAUL_PREFIX)/bin/

-include $(DEPS)
