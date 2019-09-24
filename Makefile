PROJECTS:= \
	libyaul \
	libtga \
	libbcl

include env.mk

THIS_ROOT:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
export THIS_ROOT

define macro-generate-build-rule
.PHONY: $1-$2

$1-$2:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk $2) || exit $${?}
endef

define macro-generate-install-rule
.PHONY: $1-install-$2

$1-install-$2:
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/lib
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/include
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)install-$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk install-$2) || exit $${?}
endef

define macro-generate-clean-rule
.PHONY: $1-clean-$2

$1-clean-$2: $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)clean-$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk clean) || exit $${?}
endef

define macro-install
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/lib
	$(ECHO)mkdir -p $(YAUL_INSTALL_ROOT)/include
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f $<.mk $@) || exit $${?}; \
	done
endef

define macro-check-toolchain
	$(ECHO)for tool in $2; do \
	    printf -- "$(YAUL_INSTALL_ROOT)/bin/$1-$${tool}$(EXE_EXT)\n"; \
	    if [ ! -e $(YAUL_INSTALL_ROOT)/bin/$1-$${tool} ]; then \
		printf -- "$1 toolchain has not been installed properly (see build-scripts/)\n" >&2; \
		exit 1; \
	    fi; \
	done
endef

.PHONY: all \
	release \
	debug \
	install \
	install-release \
	install-debug \
	distclean \
	create-pacman-package \
	clean \
	clean-release \
	clean-debug \
	examples \
	clean-examples \
	tools \
	install-tools \
	clean-tools \
	list-targets \
	check-toolchain

all: release debug tools

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),debug)))

check-toolchain:
	@printf -- "$(V_BEGIN_YELLOW)toolchain check$(V_END)\n"
	$(call macro-check-toolchain,$(YAUL_ARCH_SH_PREFIX),as ar ld nm objcopy objdump gcc g++)
	$(call macro-check-toolchain,$(M68K_ARCH),as ar ld nm objcopy objdump)

release debug: $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f $@.mk $@) || exit $${?}; \
	done

$(YAUL_BUILD_ROOT)/$(YAUL_BUILD):
	@$(MAKE) -s check-toolchain
	$(ECHO)mkdir -p $@

install: install-release install-tools

install-release: release
	$(call macro-install)

install-debug: debug
	$(call macro-install)

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),debug)))

create-pacman-package:
	@echo "X"

clean: clean-release clean-debug clean-tools

distclean: clean-examples
	$(ECHO)$(RM) -r $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)

clean-release:
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f release.mk clean) || exit $${?}; \
	done

clean-debug:
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f debug.mk clean) || exit $${?}; \
	done

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),debug)))

examples:
	$(ECHO)($(MAKE) -C examples all) || exit $${?}

clean-examples:
	$(ECHO)($(MAKE) -C examples clean) || exit $${?}

tools:
	$(ECHO)($(MAKE) -C tools all) || exit $${?}

install-tools: tools
	$(ECHO)($(MAKE) -C tools install) || exit $${?}

clean-tools:
	$(ECHO)($(MAKE) -C tools clean) || exit $${?}

list-targets:
	@$(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'
