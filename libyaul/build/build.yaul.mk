YAUL:= yaul

YAUL_CFLAGS_shared:= -I$(YAUL_INSTALL_ROOT)/$(YAUL_ARCH_SH_PREFIX)/include/yaul

YAUL_CFLAGS:= $(YAUL_CFLAGS_shared)
YAUL_CXXFLAGS:= $(YAUL_CFLAGS_shared)

YAUL_LDFLAGS:=
