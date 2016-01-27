TARGET:= yaul

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

ifeq ($(strip $(BUILD_ROOT)),)
  $(error Undefined BUILD_ROOT (build root directory))
endif

ifeq ($(strip $(BUILD)),)
  $(error Undefined BUILD (build directory))
endif

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
endif

SUB_BUILD:=$(BUILD)/lib$(TARGET)

BOOTSTRAP_FILES:= \
	common/bootstrap/ip.S \
	common/bootstrap/sys_aree.bin \
	common/bootstrap/sys_arej.bin \
	common/bootstrap/sys_aret.bin \
	common/bootstrap/sys_areu.bin \
	common/bootstrap/sys_init.bin \
	common/bootstrap/sys_sec.bin

USER_FILES:= \
	common/pre.common.mk \
	common/post.common.mk

LDSCRIPTS:= \
	common/ldscripts/yaul.x \
	common/ldscripts/ip.x

SPECS:= \
	common/specs/yaul.specs \
	common/specs/ip.specs

START_SRCS:= \
	common/crt0.S

LIB_SRCS:= \
	common/abort.c \
	common/assert.c \
	common/exception.S \
	common/exception_handler.c \
	common/irq-mux.c \
	common/stack.c \
	\
	common/gdb/gdb.c \
	common/gdb/ihr.S \
	common/gdb/sh2-704x.c \
	\
	cons/cons.c \
	cons/drivers/vdp1.c \
	cons/drivers/vdp2.c \
	cons/font.c \
	cons/vt_parse/vt_parse.c \
	cons/vt_parse/vt_parse_table.c \
	\
	kernel/init.c \
	\
	kernel/lib/memb.c \
	\
	kernel/mm/free.c \
	kernel/mm/free_r.c \
	kernel/mm/malloc.c \
	kernel/mm/malloc_r.c \
	kernel/mm/realloc.c \
	kernel/mm/realloc_r.c \
	kernel/mm/slob.c \
	\
	kernel/vfs/pathname_cache.c \
	\
	kernel/vfs/fs/iso9660/iso9660.c \
	\
	kernel/vfs/fs/romdisk/romdisk.c \
	\
	newlib/chmod_r.c \
	newlib/chown_r.c \
	newlib/close_r.c \
	newlib/creat_r.c \
	newlib/execve_r.c \
	newlib/exit.c \
	newlib/fork_r.c \
	newlib/fstat_r.c \
	newlib/getpid_r.c \
	newlib/isatty_r.c \
	newlib/kill_r.c \
	newlib/lseek_r.c \
	newlib/open_r.c \
	newlib/pipe_r.c \
	newlib/raise_r.c \
	newlib/read_r.c \
	newlib/sbrk_r.c \
	newlib/stat_r.c \
	newlib/unlink_r.c \
	newlib/utime_r.c \
	newlib/wait_r.c \
	newlib/write_r.c \
	\
	scu/bus/a/cs0/arp/arp_busy_status.c \
	scu/bus/a/cs0/arp/arp_function_callback.c \
	scu/bus/a/cs0/arp/arp_function_nonblock.c \
	scu/bus/a/cs0/arp/arp_read_byte.c \
	scu/bus/a/cs0/arp/arp_read_long.c \
	scu/bus/a/cs0/arp/arp_send_long.c \
	scu/bus/a/cs0/arp/arp_sync.c \
	scu/bus/a/cs0/arp/arp_sync_nonblock.c \
	scu/bus/a/cs0/arp/arp_version.c \
	scu/bus/a/cs0/arp/arp_xchg_byte.c \
	\
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_area.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_id.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_init.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_size.c \
	\
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_init.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_read_byte.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_read_long.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_return.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_send_byte.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_send_long.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_xchg_byte.c \
	\
	scu/bus/a/cs2/cd-block/cd-block_cmd_abort_file.c \
	scu/bus/a/cs2/cd-block/cd-block_cmd_init_cd_system.c \
	scu/bus/a/cs2/cd-block/cd-block_execute.c \
	scu/bus/a/cs2/cd-block/cd-block_init.c \
	\
	scu/bus/b/scsp/scsp_init.c \
	\
	scu/bus/b/vdp1/vdp1_cmdt.c \
	scu/bus/b/vdp1/vdp1_init.c \
	\
	scu/bus/b/vdp2/vdp2_init.c \
	scu/bus/b/vdp2/vdp2_priority_spn_set.c \
	scu/bus/b/vdp2/vdp2_scrn_back_screen_set.c \
	scu/bus/b/vdp2/vdp2_scrn_bitmap_format_set.c \
	scu/bus/b/vdp2/vdp2_scrn_cell_format_set.c \
	scu/bus/b/vdp2/vdp2_scrn_display_clear.c \
	scu/bus/b/vdp2/vdp2_scrn_display_set.c \
	scu/bus/b/vdp2/vdp2_scrn_display_unset.c \
	scu/bus/b/vdp2/vdp2_scrn_ls_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_clear.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_horizontal_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_unset.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_vertical_set.c \
	scu/bus/b/vdp2/vdp2_scrn_scv_x_set.c \
	scu/bus/b/vdp2/vdp2_scrn_scv_y_set.c \
	scu/bus/b/vdp2/vdp2_scrn_vcs_set.c \
	scu/bus/b/vdp2/vdp2_tvmd_display_clear.c \
	scu/bus/b/vdp2/vdp2_tvmd_display_set.c \
	scu/bus/b/vdp2/vdp2_tvmd_hblank_in_irq_get.c \
	scu/bus/b/vdp2/vdp2_tvmd_vblank_in_irq_get.c \
	scu/bus/b/vdp2/vdp2_tvmd_vblank_out_irq_get.c \
	scu/bus/b/vdp2/vdp2_vram_control_get.c \
	scu/bus/b/vdp2/vdp2_vram_control_set.c \
	scu/bus/b/vdp2/vdp2-internal.c \
	\
	scu/bus/cpu/cpu_dmac_channel_set.c \
	scu/bus/cpu/cpu_dmac_channel_start.c \
	scu/bus/cpu/cpu_dmac_channel_stop.c \
	scu/bus/cpu/cpu_intc_disable.S \
	scu/bus/cpu/cpu_intc_enable.S \
	scu/bus/cpu/cpu_intc_interrupt_get.S \
	scu/bus/cpu/cpu_intc_interrupt_set.S \
	scu/bus/cpu/cpu_intc_mask_get.S \
	scu/bus/cpu/cpu_intc_mask_set.S \
	scu/bus/cpu/cpu_intc_vector_base_get.S \
	scu/bus/cpu/cpu_intc_vector_base_set.S \
	scu/bus/cpu/cpu_intc_vector_table.S \
	scu/bus/cpu/cpu_ubc_init.c \
	\
	scu/bus/cpu/smpc/smpc_init.c \
	scu/bus/cpu/smpc/smpc_peripheral_analog_port.c \
	scu/bus/cpu/smpc/smpc_peripheral_digital_get.c \
	scu/bus/cpu/smpc/smpc_peripheral_digital_port.c \
	scu/bus/cpu/smpc/smpc_peripheral_init.c \
	scu/bus/cpu/smpc/smpc_peripheral_keyboard_port.c \
	scu/bus/cpu/smpc/smpc_peripheral_mouse_port.c \
	scu/bus/cpu/smpc/smpc_peripheral_racing_port.c \
	scu/bus/cpu/smpc/smpc_peripheral_raw_port.c \
	scu/bus/cpu/smpc/smpc_rtc_settime_call.c \
	scu/bus/cpu/smpc/smpc_smc_call.c \
	scu/bus/cpu/smpc/smpc_smc_cdoff_call.c \
	scu/bus/cpu/smpc/smpc_smc_cdon_call.c \
	scu/bus/cpu/smpc/smpc_smc_ckchg320_call.c \
	scu/bus/cpu/smpc/smpc_smc_ckchg352_call.c \
	scu/bus/cpu/smpc/smpc_smc_intback_call.c \
	scu/bus/cpu/smpc/smpc_smc_mshon_call.c \
	scu/bus/cpu/smpc/smpc_smc_nmireq_call.c \
	scu/bus/cpu/smpc/smpc_smc_resdisa_call.c \
	scu/bus/cpu/smpc/smpc_smc_resenab_call.c \
	scu/bus/cpu/smpc/smpc_smc_setsmem_call.c \
	scu/bus/cpu/smpc/smpc_smc_sndoff_call.c \
	scu/bus/cpu/smpc/smpc_smc_sndon_call.c \
	scu/bus/cpu/smpc/smpc_smc_sshoff_call.c \
	scu/bus/cpu/smpc/smpc_smc_sshon_call.c \
	scu/bus/cpu/smpc/smpc_smc_sysres_call.c \
	\
	scu/scu_dma_cpu_init.c \
	scu/scu_dma_cpu_level_set.c \
	scu/scu_dma_cpu_level_start.c \
	scu/scu_dma_cpu_level_stop.c \
	scu/scu_dsp_init.c \
	scu/scu_ic_interrupt_get.S \
	scu/scu_ic_interrupt_set.S \
	scu/scu_ic_mask_chg.S \
	scu/scu_ic_mask_get.S \
	scu/scu_ic_mask_set.S \
	scu/scu_ic_status_get.c \
	scu/scu_timer_0_set.c \
	scu/scu_timer_1_mode_clear.c \
	scu/scu_timer_1_mode_set.c \
	scu/scu_timer_1_set.c

INCLUDE_DIRS:= \
	common \
	common/gdb \
	cons \
	kernel \
	kernel/lib \
	kernel/mm \
	kernel/vfs \
	kernel/vfs/fs/iso9660 \
	kernel/vfs/fs/romdisk \
	newlib \
	scu \
	scu/bus/a/cs0/arp \
	scu/bus/a/cs0/dram-cartridge \
	scu/bus/a/cs0/usb-cartridge \
	scu/bus/a/cs2/cd-block \
	scu/bus/b/scsp \
	scu/bus/b/vdp1 \
	scu/bus/b/vdp2 \
	scu/bus/cpu \
	scu/bus/cpu/smpc

HEADER_FILES:= \
	./yaul.h:. \
	\
	./common/common.h:. \
	./common/irq-mux.h: \
	./common/stack.h:. \
	./common/gdb/gdb.h:. \
	./common/gdb/gdb-signals.h:. \
	./common/gdb/sh2-704x.h:. \
	\
	./cons/cons.h:. \
	./cons/drivers.h:. \
	./cons/font.h:. \
	./cons/vt_parse/vt_parse.h:. \
	./cons/vt_parse/vt_parse_table.h:. \
	\
	./kernel/lib/memb.h:../../lib \
	./kernel/vfs/fs/iso9660/iso9660.h:../../../../fs/iso9660 \
	./kernel/vfs/fs/romdisk/romdisk.h:../../../../fs/romdisk \
	\
	./scu/bus/a/cs0/arp/arp.h:. \
	./scu/bus/a/cs0/arp/arp-internal.h:. \
	\
	./scu/bus/a/cs0/dram-cartridge/dram-cartridge.h:. \
	./scu/bus/a/cs0/dram-cartridge/dram-cartridge-internal.h:. \
	\
	./scu/bus/a/cs0/usb-cartridge/usb-cartridge.h:. \
	./scu/bus/a/cs0/usb-cartridge/usb-cartridge-internal.h:. \
	\
	./scu/bus/a/cs2/cd-block/cd-block.h:. \
	./scu/bus/a/cs2/cd-block/cd-block-internal.h:. \
	./scu/bus/a/cs2/cd-block/cd-block/cmd.h:. \
	\
	./scu/bus/b/scsp/scsp.h:. \
	./scu/bus/b/scsp/scsp.h:scsp-internal.h. \
	\
	./scu/bus/b/vdp1/vdp1.h:. \
	./scu/bus/b/vdp1/vdp1-internal.h:. \
	./scu/bus/b/vdp1/vdp1/cmdt.h:. \
	./scu/bus/b/vdp1/vdp1/map.h:. \
	./scu/bus/b/vdp1/vdp1/vram.h:. \
	\
	./scu/bus/b/vdp2/vdp2.h:. \
	./scu/bus/b/vdp2/vdp2-internal.h:. \
	./scu/bus/b/vdp2/vdp2/cram.h:. \
	./scu/bus/b/vdp2/vdp2/map.h:. \
	./scu/bus/b/vdp2/vdp2/pn.h:. \
	./scu/bus/b/vdp2/vdp2/priority.h:. \
	./scu/bus/b/vdp2/vdp2/scrn.h:. \
	./scu/bus/b/vdp2/vdp2/tvmd.h:. \
	./scu/bus/b/vdp2/vdp2/vram.h:. \
	\
	./scu/bus/cpu/cpu.h:. \
	./scu/bus/cpu/cpu-internal.h:. \
	./scu/bus/cpu/cpu/dmac.h:. \
	./scu/bus/cpu/cpu/intc.h:. \
	./scu/bus/cpu/cpu/ubc.h:. \
	./scu/bus/cpu/cpu/registers.h:. \
	\
	./scu/bus/cpu/smpc/smpc.h:. \
	./scu/bus/cpu/smpc/smpc-internal.h:. \
	./scu/bus/cpu/smpc/smpc/peripheral.h:. \
	./scu/bus/cpu/smpc/smpc/rtc.h:. \
	./scu/bus/cpu/smpc/smpc/smc.h:. \
	\
	./scu/scu.h:. \
	./scu/scu-internal.h:. \
	./scu/scu/dma.h:. \
	./scu/scu/dsp.h:. \
	./scu/scu/ic.h:. \
	./scu/scu/map.h:. \
	./scu/scu/timer.h:.

SILENT?= @
V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

ARCH?= sh-elf

CXX:= $(ARCH)-g++
AS:= $(ARCH)-as
AR:= $(ARCH)-ar
CC:= $(ARCH)-gcc
LD:= $(ARCH)-gcc
NM:= $(ARCH)-nm
OBJCOPY:= $(ARCH)-objcopy
OBJDUMP:= $(ARCH)-objdump
SED:= sed
FIND:= find
INSTALL:= install

CFLAGS:= -std=c99 \
	-s \
	-ffast-math \
	-fstrict-aliasing \
	-fomit-frame-pointer \
	-Wall \
	-Wuninitialized \
	-Winit-self \
	-Wuninitialized \
	-Wshadow \
	-Wno-unused \
	-Wno-parentheses \
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE)
CFLAGS_release:= -O2 $(CFLAGS)
CFLAGS_release-internal:= -O2 $(CFLAGS) \
	-DRELEASE_INTERNAL
CFLAGS_debug:= -O0 -g $(CFLAGS) \
	-DDEBUG

START_OBJS:= $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(START_SRCS)))
START_DEPS:= $(START_OBJS:.o=.d)

START_OBJS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(START_OBJS))
START_OBJS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(START_OBJS))
START_OBJS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(START_OBJS))
START_DEPS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(START_DEPS))
START_DEPS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(START_DEPS))
START_DEPS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(START_DEPS))

LIB_OBJS:= $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(LIB_SRCS)))
LIB_DEPS:= $(LIB_OBJS:.o=.d)

LIB_OBJS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(LIB_OBJS))
LIB_OBJS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(LIB_OBJS))
LIB_OBJS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(LIB_OBJS))
LIB_DEPS_release:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release/,$(LIB_DEPS))
LIB_DEPS_release-internal:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/,$(LIB_DEPS))
LIB_DEPS_debug:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/debug/,$(LIB_DEPS))

HEADER_FILES_all:= $(addprefix $(BUILD_ROOT)/lib$(TARGET)/,$(HEADER_FILES))

LDSCRIPTS_all:= $(addprefix $(BUILD_ROOT)/$(SUB_BUILD)/,$(LDSCRIPTS))
SPECS_all := $(addprefix $(BUILD_ROOT)/lib$(TARGET)/,$(SPECS))
BOOTSTRAP_FILES_all = $(addprefix $(BUILD_ROOT)/lib$(TARGET)/,$(BOOTSTRAP_FILES))
USER_FILES_all = $(addprefix $(BUILD_ROOT)/lib$(TARGET)/,$(USER_FILES))

define macro-build-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)mkdir -p $(@D)
	$(SILENT)$(CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(CFLAGS_$1) \
		$(foreach dir,$(INCLUDE_DIRS),-I./$(dir)) \
		-c $< -o $@
	$(SILENT)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-build-library
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)$(AR) rcs $@ $(LIB_OBJS_$1)
endef

define macro-install-library
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$1/lib$(TARGET).a$(V_END)\n"
	$(SILENT)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/$1/lib$(TARGET).a $(INSTALL_ROOT)/$(ARCH)/lib/
endef

define macro-install-start-scripts
	$(SILENT)for object in $(START_OBJS_$1); do \
	    printf -- "$(V_BEGIN_BLUE)$${object#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$object $(INSTALL_ROOT)/$(ARCH)/lib/; \
	done
endef

define macro-install-files
	$(SILENT)$(RM) -r $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)
	$(SILENT)mkdir -p $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)
	$(SILENT)for pair in $(HEADER_FILES_all); do \
	    header_file=$$(echo "$${pair}" | cut -d ':' -f 1); \
	    rel_dst=$$(echo "$${pair}" | cut -d ':' -f 2); \
            rel_header_file=$${header_file#$(BUILD_ROOT)/lib$(TARGET)/}; \
	    rel_header_dir=$${rel_header_file%/*}/$${rel_dst}; \
	    install_header_file=$$(realpath -m $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)/$${rel_header_dir}/$${rel_header_file##*/}); \
	    install_header_dir=$$(realpath -m $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)/$${rel_header_dir}); \
	    printf -- "$(V_BEGIN_BLUE)lib$(TARGET)/$${header_file#$(BUILD_ROOT)/lib$(TARGET)/./}$(V_END)\n"; \
	    mkdir -p $${install_header_dir}; \
	    $(INSTALL) -m 644 $${header_file} $${install_header_file}; \
	done
	$(SILENT)for ldscript in $(LDSCRIPTS_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${ldscript#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$ldscript $(INSTALL_ROOT)/$(ARCH)/lib/ldscripts/; \
	done
	$(SILENT)for specs in $(SPECS_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${specs#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$specs $(INSTALL_ROOT)/$(ARCH)/lib/; \
	done
	$(SILENT)mkdir -p $(INSTALL_ROOT)/share/$(TARGET)/bootstrap
	$(SILENT)for bootstrap_file in $(BOOTSTRAP_FILES_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${bootstrap_file#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$bootstrap_file $(INSTALL_ROOT)/share/$(TARGET)/bootstrap; \
	done
	$(SILENT)mkdir -p $(INSTALL_ROOT)/share/$(TARGET)
	$(SILENT)for user_file in $(USER_FILES_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${user_file#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$user_file $(INSTALL_ROOT)/share/$(TARGET)/; \
	done
endef

define macro-clean
	$(SILENT)if [ -d $(BUILD_ROOT)/$(SUB_BUILD)/$1 ]; then \
		$(FIND) $(BUILD_ROOT)/$(SUB_BUILD)/$1 -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
endef

.PHONY: all release release-internal debug clean clean-release clean-release-internal clean-debug install-release install-release-internal install-debug

.SUFFIXES:= .c .S .o .x

all: release release-internal debug

release: $(BUILD_ROOT)/$(SUB_BUILD)/release $(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a $(START_OBJS_release) $(LDSCRIPTS_all) $(SPECS_all)
release-internal: $(BUILD_ROOT)/$(SUB_BUILD)/release-internal $(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a $(START_OBJS_release-internal) $(LDSCRIPTS_all) $(SPECS_all)
debug: $(BUILD_ROOT)/$(SUB_BUILD)/debug $(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a $(START_OBJS_debug) $(LDSCRIPTS_all) $(SPECS_all)

install-release: release
	$(call macro-install-files)
	$(call macro-install-start-scripts,release)
	$(call macro-install-library,release)

install-release-internal: release-internal
	$(call macro-install-start-scripts,release-internal)
	$(call macro-install-library,release-internal)
	$(call macro-install-files)

install-debug: debug
	$(call macro-install-start-scripts,debug)
	$(call macro-install-library,debug)
	$(call macro-install-files)

$(BUILD_ROOT)/$(SUB_BUILD)/release:
	$(SILENT)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal:
	$(SILENT)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/debug:
	$(SILENT)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a: $(LIB_OBJS_release)
	$(call macro-build-library,release)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a: $(LIB_OBJS_release-internal)
	$(call macro-build-library,release-internal)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a: $(LIB_OBJS_debug)
	$(call macro-build-library,debug)

$(BUILD_ROOT)/$(SUB_BUILD)/%.x: %.x
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(SILENT)mkdir -p $(@D)
	$(SILENT)cat $< | awk '/^SEARCH_DIR[[:space:]]+(.+);$$/ { \
	    sub(/\$$INSTALL_ROOT/,"$(INSTALL_ROOT)"); \
	} \
	{ print }' > $@

$(BUILD_ROOT)/$(SUB_BUILD)/release/%.o: %.c
	$(call macro-build-object,release)

$(BUILD_ROOT)/$(SUB_BUILD)/release/%.o: %.S
	$(call macro-build-object,release)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/%.o: %.c
	$(call macro-build-object,release-internal)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/%.o: %.S
	$(call macro-build-object,release-internal)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/%.o: %.c
	$(call macro-build-object,debug)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/%.o: %.S
	$(call macro-build-object,debug)

clean: clean-release clean-release-internal clean-debug

clean-release:
	$(call macro-clean,release)

clean-release-internal:
	$(call macro-clean,release-internal)

clean-debug:
	$(call macro-clean,debug)

-include $(START_DEPS_release)
-include $(START_DEPS_debug)
-include $(START_DEPS_release-internal)

-include $(LIB_DEPS_release)
-include $(LIB_DEPS_debug)
-include $(LIB_DEPS_release-internal)
