PROJECTS:= \
	bin2c \
	bin2o \
	make-cue \
	make-iso \
	make-ip \
	ssload

include ../env.mk

# XXX: Currently, gdb-ftdi-proxy is broken under Windows
ifneq ($(OS), Windows_NT)
ifeq ($(strip $(BUILD_CROSS)),)
PROJECTS+= \
	gdb-ftdi-proxy
endif
endif

.PHONY: all clean distclean install

all clean distclean install:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for tool in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)tools $${tool}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
		($(MAKE) -C $${tool} $@) || exit $${?}; \
	done
