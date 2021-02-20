TARGET:= bin2o

include ../../env.mk

.PHONY: all clean distclean install

all clean distclean:

install: $(TARGET)
	@printf -- "$(V_BEGIN_BLUE)$(TARGET)$(V_END)\n"
	$(ECHO)mkdir -p $(YAUL_PREFIX)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(YAUL_PREFIX)/bin/
