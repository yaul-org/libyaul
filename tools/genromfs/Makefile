ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

TARGET:= genromfs

SILENT?= @

INSTALL:= install

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

OBJS:= $(SRCS:.c=.o)
DEPS:= $(SRCS:.c=.d)

.PHONY: all clean distclean install

all: $(TARGET)

$(TARGET): $(OBJS)
	@printf -- "[1;33m$@[m\n"
	$(CC) -o $(TARGET) $^

%.o: %.c
	@printf -- "[1;33m$@[m\n"
	$(CC) -Wp,-MMD,$*.d $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)

distclean: clean

install: $(TARGET)
	@printf -- "[1;34m$^[m\n"
	$(SILENT)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/
	@printf -- "[1;34mfsck.genromfs[m\n"
	$(SILENT)$(INSTALL) -m 755 fsck.genromfs $(INSTALL_ROOT)/bin/

-include $(DEPS)
