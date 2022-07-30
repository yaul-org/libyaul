PROJECTS:= \
	bin2c \
	bin2o \
	make-cue \
	make-iso \
	make-ip

include ../env.mk

.PHONY: all clean distclean install

all clean distclean install:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for tool in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)tools $${tool}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
		($(MAKE) -C $${tool} $@) || exit $${?}; \
	done
