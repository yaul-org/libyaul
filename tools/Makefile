PROJECTS:= \
	bin2c \
	bin2o \
	genromfs \
	make-iso \
	make-ip

include ../env.mk

# Build ssload only if we have a development cartridge to use
ifneq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),0)
PROJECTS+= \
	gdb-ftdi-proxy \
	ssload
endif

.PHONY: all clean distclean install

all clean distclean install:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for tool in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)tools $${tool}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
		($(MAKE) -C $${tool} $@) || exit $${?}; \
	done
