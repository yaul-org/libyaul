PROJECTS:= \
	bcl \
	bin2c \
	bin2o \
	genromfs \
	make-iso

include ../env.mk

# Build ssload only if we have a development cartridge to use
ifneq ($(strip $(OPTION_DEV_CARTRIDGE)),0)
PROJECTS+= \
	ssload
endif

.PHONY: all clean distclean install

all clean distclean install:
	$(ECHO)mkdir -p $(BUILD_ROOT)/$(BUILD)
	$(ECHO)for tool in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)tools/$$tool$(V_END)\n"; \
		($(MAKE) -C $$tool $@) || exit $$?; \
	done
