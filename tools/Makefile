PROJECTS:= \
	bin2c \
	bin2o \
	fsck.genromfs \
	make-iso

SILENT?= @

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

.PHONY: all clean distclean install

all clean distclean install:
	$(SILENT)mkdir -p $(BUILD_ROOT)/$(BUILD)
	$(SILENT)for tool in $(PROJECTS); do \
		printf -- "[1;36m$@[m [1;32mtools/$$tool[m\n"; \
		($(MAKE) -C $$tool $@) || exit $$?; \
	done
