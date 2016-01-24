ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

TARGET:= bin2o

SILENT?= @

INSTALL:= install

.PHONY: all clean distclean install

all clean distclean:

install: $(TARGET)
	@printf -- "[1;34m$^[m\n"
	$(SILENT)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/
