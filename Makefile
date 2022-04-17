PROJECTS:= \
	libyaul \
	libtga \
	libbcl \
	libg3d

include env.mk

THIS_ROOT:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
export THIS_ROOT

# $1 -> Project directory
# $2 -> Project name
define macro-generate-build-rule
.PHONY: $1-$2

$1-$2:
	$(ECHO)mkdir -p $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk $2) || exit $${?}
endef

# $1 -> Project directory
# $2 -> Project name
define macro-generate-install-rule
.PHONY: $1-install-$2

$1-install-$2:
	$(ECHO)mkdir -p $(YAUL_PREFIX)/lib
	$(ECHO)mkdir -p $(YAUL_PREFIX)/include
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)install-$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk install-$2) || exit $${?}
endef

# $1 -> Project directory
define macro-generate-generate-cdb-rule
.PHONY: $1-generate-cdb

$1-generate-cdb:
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)generate-cdb$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f release.mk generate-cdb) || exit $${?}
endef

# $1 ->
# $2 ->
define macro-generate-clean-rule
.PHONY: $1-clean-$2

$1-clean-$2: $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)printf -- "$(V_BEGIN_CYAN)$1$(V_END) $(V_BEGIN_GREEN)clean-$2$(V_END)\n"
	$(ECHO)($(MAKE) -C $1 -f $2.mk clean) || exit $${?}
endef

# No arguments
define macro-install
	$(ECHO)mkdir -p $(YAUL_PREFIX)/lib
	$(ECHO)mkdir -p $(YAUL_PREFIX)/include
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f $<.mk $@) || exit $${?}; \
	done
endef

# $1 ->
# $2 ->
define macro-check-tool-chain
	$(ECHO)for tool in $2; do \
	    printf -- "$(YAUL_INSTALL_ROOT)/bin/$1-$${tool}$(EXE_EXT)\n"; \
	    if [ ! -e $(YAUL_INSTALL_ROOT)/bin/$1-$${tool} ]; then \
		printf -- "$1 tool-chain has not been installed properly (see build-scripts/)\n" >&2; \
		exit 1; \
	    fi; \
	done
endef

.PHONY: all \
	check-tool-chain \
	clean \
	clean-cdb \
	clean-debug \
	clean-examples \
	clean-release \
	clean-tools \
	debug \
	distclean \
	examples \
	generate-cdb \
	install \
	install-debug \
	install-release \
	install-tools \
	list-targets \
	release \
	tools

all: release debug tools examples

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-build-rule,$(project),debug)))

check-tool-chain:
	@printf -- "$(V_BEGIN_YELLOW)tool-chain check$(V_END)\n"
	$(call macro-check-tool-chain,$(YAUL_ARCH_SH_PREFIX),as ar ld nm objcopy objdump gcc g++)

release debug: $(YAUL_BUILD_ROOT)/$(YAUL_BUILD)
	$(ECHO)for project in $(PROJECTS); do \
	    printf -- "$(V_BEGIN_CYAN)$${project}$(V_END) $(V_BEGIN_GREEN)$@$(V_END)\n"; \
	    ($(MAKE) -C $${project} -f $@.mk $@) || exit $${?}; \
	done

$(YAUL_BUILD_ROOT)/$(YAUL_BUILD):
	@$(MAKE) -s check-tool-chain
	$(ECHO)mkdir -p $@

install: install-release install-tools

install-release: release
	$(call macro-install)

install-debug: debug
	$(call macro-install)

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),release)))
$(foreach project,$(PROJECTS),$(eval $(call macro-generate-install-rule,$(project),debug)))

clean: clean-release clean-debug clean-tools clean-examples clean-cdb

distclean: clean-examples clean-cdb
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

$(foreach project,$(PROJECTS),$(eval $(call macro-generate-generate-cdb-rule,$(project))))

generate-cdb: clean-cdb $(patsubst %,%-generate-cdb,$(PROJECTS))

clean-cdb:
	$(RM) $(CDB_FILE)

list-targets:
	@$(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'
