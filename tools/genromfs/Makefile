include ../../env.mk

TARGET:= genromfs
PROGRAM:= $(TARGET)$(EXE_EXT)

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
	-Wno-parentheses

SRCS:= genromfs.c
INCLUDES:= /usr/include

OBJS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/,$(SRCS:.c=.d))

.PHONY: all clean distclean install

all: $(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM)

$(YAUL_BUILD_ROOT)/$(SUB_BUILD)/$(PROGRAM): $(YAUL_BUILD_ROOT)/$(SUB_BUILD) $(OBJS)
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(YAUL_BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(CC) -o $@ $(OBJS)
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
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(YAUL_INSTALL_ROOT)/bin/
	@printf -- "$(V_BEGIN_BLUE)fsck.genromfs$(V_END)\n"
	$(ECHO)$(INSTALL) -m 755 fsck.genromfs $(YAUL_INSTALL_ROOT)/bin/

-include $(DEPS)
