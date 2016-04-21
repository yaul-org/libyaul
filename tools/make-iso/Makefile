TARGET:= make-iso

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(SILENT)),1)
  ECHO=@
else
  ECHO=
endif
export ECHO

V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

INSTALL:= install

.PHONY: all clean distclean install

all clean distclean:

install: $(TARGET)
	@printf -- "$(V_BEGIN_BLUE)$(TARGET)$(V_END)\n"
	$(ECHO)mkdir -p $(INSTALL_ROOT)/bin
	$(ECHO)$(INSTALL) -m 755 $< $(INSTALL_ROOT)/bin/
