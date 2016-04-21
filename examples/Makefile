EXAMPLES:= \
	games/inkfill \
	simple/arp-comm \
	simple/arp-msg \
	simple/cpu-dmac \
	simple/dram-cartridge \
	simple/gdb \
	simple/line-scroll-screen \
	simple/romdisk \
	test-suites/vdp1

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(SILENT)),1)
  ECHO=@
else
  ECHO=
endif
export ECHO

.PHONY: all clean

all clean:
	$(ECHO)for example in $(EXAMPLES); do \
		printf -- "[1;36m$@[m [1;32mexamples/$$example[m\n"; \
		($(MAKE) -C $$example $@) || exit $$?; \
	done
