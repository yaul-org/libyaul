PROJECTS:= \
	libyaul \
	libfixmath \
	libtga

BUILD?= build
SILENT?= @

ifndef BUILD_ROOT
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifndef INSTALL_ROOT
  $(error Undefined INSTALL_ROOT (install root directory))
endif

define macro-install
	$(SILENT)mkdir -p $(INSTALL_ROOT)/lib
	$(SILENT)mkdir -p $(INSTALL_ROOT)/include
	$(SILENT)for project in $(PROJECTS); do \
		printf -- "[1;36m$@[m [1;32m$$project[m\n"; \
		($(MAKE) BUILD=$(BUILD) -C $$project $@) || exit $$?; \
	done
endef

.PHONY: all release release-internal debug clean clean-release clean-release-internal clean-debug examples

all: release release-internal debug examples

release release-internal debug clean clean-release clean-release-internal clean-debug:
	$(SILENT)mkdir -p $(BUILD_ROOT)/$(BUILD)
	$(SILENT)for project in $(PROJECTS); do \
		printf -- "[1;36m$@[m [1;32m$$project[m\n"; \
		($(MAKE) BUILD=$(BUILD) -C $$project $@) || exit $$?; \
	done

install-release: release
	$(call macro-install)

install-release-internal: release-internal
	$(call macro-install)

install-debug: debug
	$(call macro-install)

examples:
	$(SILENT)($(MAKE) -C examples all) || exit $$?
