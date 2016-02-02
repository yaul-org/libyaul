EXAMPLES:= \
	games/dark-blue \
	games/inkfill \
	simple/arp-comm \
	simple/arp-msg \
	simple/cpu-dmac \
	simple/dram-cartridge \
	simple/gdb \
	simple/line-scroll-screen \
	simple/romdisk \
	test-suites/vdp1

SILENT?= @

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

.PHONY: all clean

all clean:
	$(SILENT)for example in $(EXAMPLES); do \
		printf -- "[1;36m$@[m [1;32mexamples/$$example[m\n"; \
		($(MAKE) -C $$example $@) || exit $$?; \
	done
