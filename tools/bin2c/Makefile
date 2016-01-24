ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

TARGET:= bin2c

BUILD?= build

SILENT?= @

CCFLAGS:= \
	-O2 \
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

SED:= sed
INSTALL:= install
STRIP:= strip

SRCS:= bin2c.c

OBJS:= $(addprefix $(BUILD)/,$(SRCS:.c=.o))
DEPS:= $(addprefix $(BUILD)/,$(SRCS:.c=.d))

.PHONY: all install clean distclean

all: $(BUILD) $(BUILD)/$(TARGET)

install: $(BUILD)/$(TARGET)
	@printf -- "[1;34m$^[m\n"
	$(SILENT)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/

clean:
	$(RM) $(OBJS) $(DEPS) $(BUILD)/$(TARGET)

distclean: clean
	$(RM) -r $(BUILD)

$(BUILD):
	mkdir -p $@

$(BUILD)/$(TARGET): $(OBJS)
	@printf -- "[1;33m$@[m\n"
	$(SILENT)$(CC) -o $@ $(OBJS) $(LDFLAGS)
	$(SILENT)$(STRIP) -s $@

$(BUILD)/%.o: %.c
	@printf -- "[1;33m$@[m\n"
	$(SILENT)$(CC) -Wp,-MMD,$(BUILD)/$*.d $(CCFLAGS) -c -o $@ $<
	$(SILENT)$(SED) -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD)/$*.d \
		> $(BUILD)/$*.d

-include $(DEPS)
