PROJECTS:= \
	libyaul \
	libbcl \
	libfixmath \
	libtga

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

SILENT?= @
V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

define macro-install
	$(SILENT)mkdir -p $(INSTALL_ROOT)/lib
	$(SILENT)mkdir -p $(INSTALL_ROOT)/include
	$(SILENT)for project in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
		($(MAKE) BUILD=$(BUILD) -C $$project $@) || exit $$?; \
	done
endef

.PHONY: all release release-internal debug install install-release install-release-internal install-debug clean clean-release clean-release-internal clean-debug examples clean-examples tools install-tools clean-tools list-targets

all: release release-internal debug tools

release release-internal debug:
	$(SILENT)mkdir -p $(BUILD_ROOT)/$(BUILD)
	$(SILENT)for project in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
		($(MAKE) BUILD=$(BUILD) -C $$project $@) || exit $$?; \
	done

install: install-release install-tools

install-release: release
	$(call macro-install)

install-release-internal: release-internal
	$(call macro-install)

install-debug: debug
	$(call macro-install)

clean: clean-release clean-tools

distclean: clean-release clean-release-internal clean-debug clean-tools clean-examples
	$(RM) -r $(BUILD_ROOT)/$(BUILD)

clean-release clean-release-internal clean-debug:
	$(SILENT)for project in $(PROJECTS); do \
		printf -- "$(V_BEGIN_CYAN)$@$(V_END) $(V_BEGIN_GREEN)$$project$(V_END)\n"; \
		($(MAKE) BUILD=$(BUILD) -C $$project $@) || exit $$?; \
	done

examples:
	$(SILENT)($(MAKE) -C examples all) || exit $$?

clean-examples:
	$(SILENT)($(MAKE) -C examples clean) || exit $$?

tools:
	$(SILENT)($(MAKE) -C tools all) || exit $$?

install-tools: tools
	$(SILENT)($(MAKE) -C tools install) || exit $$?

clean-tools:
	$(SILENT)($(MAKE) -C tools clean) || exit $$?

list-targets:
	@$(MAKE) -pRrq -f $(firstword $(MAKEFILE_LIST)) : 2>/dev/null | \
	awk -v RS= -F: '/^# File/,/^# Finished Make data base/ {if ($$1 !~ "^[#.]") {print $$1}}' | \
	sort | \
	grep -E -v -e '^[^[:alnum:]]' -e '^$@$$'
