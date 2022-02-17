PROJECTS:= \
	bin2c \
	bin2o \
	make-cue \
	make-iso \
	make-ip

include ../env.mk

# Build ssload only if we have a development cartridge to use
ifneq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),0)
PROJECTS+= \
	ssload

# XXX: Currently, gdb-ftdi-proxy is broken under Windows
ifneq ($(OS), Windows_NT)
ifeq ($(strip $(BUILD_CROSS)),)
PROJECTS+= \
	gdb-ftdi-proxy
endif
endif
endif

.PHONY: all clean distclean install

all clean distclean install:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for tool in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)tools $${tool}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
		($(MAKE) -C $${tool} $@) || exit $${?}; \
	done
