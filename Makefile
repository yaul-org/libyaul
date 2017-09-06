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
	mkdir -p $(BUILD_ROOT)/$(BUILD)
	printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)$1$(V_END)\n"
	($(MAKE) -C $1 $2) || exit $$?
endef

define macro-generate-install-rule
.PHONY: $1-$2

$1-$2:
	$(ECHO)mkdir -p $(INSTALL_ROOT)/lib
	$(ECHO)mkdir -p $(INSTALL_ROOT)/include
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)$1$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 $2) || exit $$?
endef

define macro-generate-clean-rule
.PHONY: $1-$2

$1-$2: $(BUILD_ROOT)/$(BUILD)
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)$1$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 $2) || exit $$?
endef

define macro-install
	$(ECHO)mkdir -p $(INSTALL_ROOT)/lib
	$(ECHO)mkdir -p $(INSTALL_ROOT)/include
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
	    ($(MAKE) -C $$project $@) || exit $$?; \
	done
endef

define macro-check-toolchain
	$(ECHO)for tool in $2; do \
	    printf -- "$(INSTALL_ROOT)/$1/bin/$1-$${tool}\n"; \
	    if [ ! -e $(INSTALL_ROOT)/$1/bin/$1-$${tool} ]; then \
	        printf -- "$1 toolchain has not been installed properly (see build-scripts/)\n" >&2; \
	        exit 1; \
	    fi; \
	done
endef

.PHONY: all \
	release \
	release-internal \
	debug \
	install \
	install-release \
	install-release-internal \
	install-debug \
	distclean \
	clean \
	clean-release \
	clean-release-internal \
	clean-debug \
	examples \
	clean-examples \
	tools \
	install-tools \
	clean-tools \
	list-targets \
	check-toolchain

all: release release-internal debug tools

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),release-internal)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),debug)))

check-toolchain:
	@printf -- "$(V_BEGIN_YELLOW)toolchain check$(V_END)\n"
	$(call macro-check-toolchain,sh-elf,as ar ld nm objcopy objdump gcc g++)
	$(call macro-check-toolchain,m68k-elf,as ar ld nm objcopy objdump)

release release-internal debug: $(BUILD_ROOT)/$(BUILD)
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
	    ($(MAKE) -C $$project $@) || exit $$?; \
	done

$(BUILD_ROOT)/$(BUILD):
	@$(MAKE) -s check-toolchain
	$(ECHO)mkdir -p $@

install: install-release install-tools

install-release: release
	$(call macro-install)

install-release-internal: release-internal
	$(call macro-install)

install-debug: debug
	$(call macro-install)

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),install-release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),install-release-internal)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),install-debug)))

clean: clean-release clean-release-internal clean-debug clean-tools

distclean: clean-examples
	$(ECHO)$(RM) -r $(BUILD_ROOT)/$(BUILD)

clean-release clean-release-internal clean-debug:
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
	    ($(MAKE) -C $$project $@) || exit $$?; \
	done

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),clean)))

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),clean-release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),clean-release-internal)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-clean-rule,$(project),clean-debug)))

examples:
	$(ECHO)($(MAKE) -C examples all) || exit $$?

clean-examples:
	$(ECHO)($(MAKE) -C examples clean) || exit $$?

tools:
	$(ECHO)($(MAKE) -C tools all) || exit $$?

install-tools: tools
	$(ECHO)($(MAKE) -C tools install) || exit $$?

clean-tools:
	$(ECHO)($(MAKE) -C tools clean) || exit $$?

list-targets:
	@$(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'
