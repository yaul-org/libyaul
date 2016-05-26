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

ifeq ($(strip $(SILENT)),1)
  ECHO=@
else
  ECHO=
endif
export ECHO

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
	common/stack.c

ifneq ($(strip $(OPTION_DEV_CARTRIDGE)),0)
LIB_SRCS+= \
	common/gdb/gdb.c \
	common/gdb/ihr.S \
	common/gdb/sh2-704x.c
endif

LIB_SRCS+= \
	cons/cons.c \
	cons/drivers/vdp1.c \
	cons/drivers/vdp2.c \
	cons/font/font.c \
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
	newlib/write_r.c

ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),2)
LIB_SRCS+= \
	scu/bus/a/cs0/arp/arp_busy_status.c \
	scu/bus/a/cs0/arp/arp_function_callback.c \
	scu/bus/a/cs0/arp/arp_function_nonblock.c \
	scu/bus/a/cs0/arp/arp_read_byte.c \
	scu/bus/a/cs0/arp/arp_read_long.c \
	scu/bus/a/cs0/arp/arp_return.c \
	scu/bus/a/cs0/arp/arp_send_long.c \
	scu/bus/a/cs0/arp/arp_sync.c \
	scu/bus/a/cs0/arp/arp_sync_nonblock.c \
	scu/bus/a/cs0/arp/arp_version.c \
	scu/bus/a/cs0/arp/arp_xchg_byte.c
endif

LIB_SRCS+= \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_area.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_id.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_init.c \
	scu/bus/a/cs0/dram-cartridge/dram-cartridge_size.c

ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),1)
LIB_SRCS+= \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_init.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_read_byte.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_read_long.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_return.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_send_byte.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_send_long.c \
	scu/bus/a/cs0/usb-cartridge/usb-cartridge_xchg_byte.c
endif

LIB_SRCS+= \
	math/color.c \
	math/fix16.c \
	math/fix16_exp.c \
	math/fix16_matrix3.c \
	math/fix16_matrix4.c \
	math/fix16_sqrt.c \
	math/fix16_str.c \
	math/fix16_trig.c \
	math/fix16_vector2.c \
	math/fix16_vector3.c \
	math/fix16_vector4.c \
	math/fract32.c \
	math/int16_vector2.c \
	math/uint32.c \
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
	scu/bus/b/vdp2/vdp2-internal.c \
	scu/bus/b/vdp2/vdp2_init.c \
	scu/bus/b/vdp2/vdp2_scrn_back_screen_addr_set.c \
	scu/bus/b/vdp2/vdp2_scrn_back_screen_color_set.c \
	scu/bus/b/vdp2/vdp2_scrn_bitmap_format_set.c \
	scu/bus/b/vdp2/vdp2_scrn_cell_format_get.c \
	scu/bus/b/vdp2/vdp2_scrn_cell_format_set.c \
	scu/bus/b/vdp2/vdp2_scrn_color_offset_clear.c \
	scu/bus/b/vdp2/vdp2_scrn_color_offset_rgb_set.c \
	scu/bus/b/vdp2/vdp2_scrn_color_offset_set.c \
	scu/bus/b/vdp2/vdp2_scrn_color_offset_unset.c \
	scu/bus/b/vdp2/vdp2_scrn_display_clear.c \
	scu/bus/b/vdp2/vdp2_scrn_display_set.c \
	scu/bus/b/vdp2/vdp2_scrn_display_unset.c \
	scu/bus/b/vdp2/vdp2_scrn_ls_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_clear.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_horizontal_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_set.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_unset.c \
	scu/bus/b/vdp2/vdp2_scrn_mosaic_vertical_set.c \
	scu/bus/b/vdp2/vdp2_scrn_priority_set.c \
	scu/bus/b/vdp2/vdp2_scrn_reduction_set.c \
	scu/bus/b/vdp2/vdp2_scrn_reduction_x_set.c \
	scu/bus/b/vdp2/vdp2_scrn_reduction_y_set.c \
	scu/bus/b/vdp2/vdp2_scrn_scroll_set.c \
	scu/bus/b/vdp2/vdp2_scrn_scroll_update.c \
	scu/bus/b/vdp2/vdp2_scrn_vcs_set.c \
	scu/bus/b/vdp2/vdp2_sprite_type_priority_get.c \
	scu/bus/b/vdp2/vdp2_sprite_type_priority_set.c \
	scu/bus/b/vdp2/vdp2_sprite_type_set.c \
	scu/bus/b/vdp2/vdp2_tvmd_display_clear.c \
	scu/bus/b/vdp2/vdp2_tvmd_display_res_set.c \
	scu/bus/b/vdp2/vdp2_tvmd_display_set.c \
	scu/bus/b/vdp2/vdp2_tvmd_hblank_in_irq_get.c \
	scu/bus/b/vdp2/vdp2_tvmd_vblank_in_irq_get.c \
	scu/bus/b/vdp2/vdp2_tvmd_vblank_out_irq_get.c \
	scu/bus/b/vdp2/vdp2_vram_control_get.c \
	scu/bus/b/vdp2/vdp2_vram_control_set.c \
	\
	scu/bus/cpu/cpu_clock_freq_chg.S \
	scu/bus/cpu/cpu_clock_freq_get.S \
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
	scu/scu_timer_1_set.c \
	scu/scu_timer_all_disable.c \
	scu/scu_timer_all_enable.c

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
	math \
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
	./math/color.h:. \
	./math/fix16.h:. \
	./math/fract32.h:. \
	./math/int16.h:. \
	./math/int64.h:. \
	./math/math.h:. \
	./math/uint32.h:. \
	\
	./common/common.h:. \
	./common/irq-mux.h:. \
	./common/stack.h:. \
	./common/bootstrap/ip.h:../

HEADER_FILES+= \
	./common/gdb/gdb.h:.

HEADER_FILES+= \
	./cons/cons.h:. \
	\
	./kernel/lib/memb.h:../../lib \
	./kernel/vfs/fs/iso9660/iso9660.h:../../../../fs/iso9660 \
	./kernel/vfs/fs/romdisk/romdisk.h:../../../../fs/romdisk

HEADER_FILES+= \
	./scu/bus/a/cs0/arp/arp.h:.

ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),2)
HEADER_FILES+= \
	./scu/bus/a/cs0/arp/arp/map.h:.
endif

HEADER_FILES+= \
	./scu/bus/a/cs0/dram-cartridge/dram-cartridge.h:. \
	./scu/bus/a/cs0/dram-cartridge/dram-cartridge/map.h:.

ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),1)
HEADER_FILES+= \
	./scu/bus/a/cs0/usb-cartridge/usb-cartridge.h:. \
	./scu/bus/a/cs0/usb-cartridge/usb-cartridge/map.h:.
endif

HEADER_FILES+= \
	./scu/bus/a/cs2/cd-block/cd-block.h:. \
	./scu/bus/a/cs2/cd-block/cd-block/cmd.h:. \
	\
	./scu/bus/b/scsp/scsp.h:. \
	\
	./scu/bus/b/vdp1/vdp1.h:. \
	./scu/bus/b/vdp1/vdp1/cmdt.h:. \
	./scu/bus/b/vdp1/vdp1/map.h:. \
	./scu/bus/b/vdp1/vdp1/vram.h:. \
	\
	./scu/bus/b/vdp2/vdp2.h:. \
	./scu/bus/b/vdp2/vdp2/cram.h:. \
	./scu/bus/b/vdp2/vdp2/map.h:. \
	./scu/bus/b/vdp2/vdp2/scrn.h:. \
	./scu/bus/b/vdp2/vdp2/sprite.h:. \
	./scu/bus/b/vdp2/vdp2/tvmd.h:. \
	./scu/bus/b/vdp2/vdp2/vram.h:. \
	\
	./scu/bus/cpu/cpu.h:. \
	./scu/bus/cpu/cpu/dmac.h:. \
	./scu/bus/cpu/cpu/intc.h:. \
	./scu/bus/cpu/cpu/map.h:. \
	./scu/bus/cpu/cpu/registers.h:. \
	./scu/bus/cpu/cpu/ubc.h:. \
	\
	./scu/bus/cpu/smpc/smpc.h:. \
	./scu/bus/cpu/smpc/smpc/peripheral.h:. \
	./scu/bus/cpu/smpc/smpc/map.h:. \
	./scu/bus/cpu/smpc/smpc/rtc.h:. \
	./scu/bus/cpu/smpc/smpc/smc.h:. \
	\
	./scu/scu.h:. \
	./scu/scu/dma.h:. \
	./scu/scu/dsp.h:. \
	./scu/scu/ic.h:. \
	./scu/scu/map.h:. \
	./scu/scu/timer.h:.

V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m

SH_AS:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-as
SH_AR:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-ar
SH_CC:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_CXX:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-g++
SH_LD:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_NM:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-nm
SH_OBJCOPY:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objcopy
SH_OBJDUMP:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objdump
M68K_AS:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-as
M68K_AR:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-ar
M68K_CC:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_CXX:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-g++
M68K_LD:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_NM:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-nm
M68K_OBJCOPY:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objcopy
M68K_OBJDUMP:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objdump
SED:= sed
FIND:= find
INSTALL:= install

SH_CFLAGS:= -std=c99 \
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
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE) \
	-DFIXMATH_NO_OVERFLOW=1 \
	-DFIXMATH_NO_ROUNDING=1
SH_CFLAGS_release:= -O2 $(SH_CFLAGS)
SH_CFLAGS_release-internal:= -O2 $(SH_CFLAGS) \
	-DRELEASE_INTERNAL
SH_CFLAGS_debug:= -O0 -g $(SH_CFLAGS) \
	-DDEBUG

M68K_CFLAGS:=

M68K_CFLAGS_release:=
M68K_CFLAGS_release-internal:=
M68K_CFLAGS_debug:=

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

HEADER_FILES_all:= $(addprefix $(THIS_ROOT)/lib$(TARGET)/,$(HEADER_FILES))

LDSCRIPTS_all:= $(LDSCRIPTS)
SPECS_all := $(addprefix $(THIS_ROOT)/lib$(TARGET)/,$(SPECS))
BOOTSTRAP_FILES_all = $(BOOTSTRAP_FILES)
USER_FILES_all = $(USER_FILES)

define macro-build-object
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)$(SH_CC) -Wp,-MMD,$(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d $(SH_CFLAGS_$1) \
		$(foreach dir,$(INCLUDE_DIRS),-I./$(dir)) \
		-c $< -o $@
	$(ECHO)$(SED) -i -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(BUILD_ROOT)/$(SUB_BUILD)/$1/$*.d
endef

define macro-build-library
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)$(SH_AR) rcs $@ $(LIB_OBJS_$1)
endef

define macro-install-library
	@printf -- "$(V_BEGIN_BLUE)$(SUB_BUILD)/$1/lib$(TARGET).a$(V_END)\n"
	$(ECHO)$(INSTALL) -m 644 $(BUILD_ROOT)/$(SUB_BUILD)/$1/lib$(TARGET).a $(INSTALL_ROOT)/sh-elf/sh-elf/lib/
endef

define macro-install-start-scripts
	$(ECHO)for object in $(START_OBJS_$1); do \
	    printf -- "$(V_BEGIN_BLUE)$${object#$(BUILD_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$object $(INSTALL_ROOT)/sh-elf/sh-elf/lib/; \
	done
endef

define macro-install-files
	$(ECHO)$(RM) -r $(INSTALL_ROOT)/$(ARCH)/include/$(TARGET)
	$(ECHO)$(RM) -r $(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)
	$(ECHO)mkdir -p $(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)
	$(ECHO)[ "$(SILENT)" != 1 ] && set -x; \
	    for pair in $(HEADER_FILES_all); do \
	    pair_header_file=$$(echo "$${pair}" | cut -d ':' -f 1); \
	    pair_rel_dst=$$(echo "$${pair}" | cut -d ':' -f 2); \
	    rel_header_file=$${pair_header_file#$(THIS_ROOT)/lib$(TARGET)/}; \
	    rel_header_dir=$${rel_header_file%/*}/$${pair_rel_dst}; \
	    path="$(INSTALL_ROOT)/sh-elf/sh-elf/include/$(TARGET)/$${rel_header_dir}/$${rel_header_file##*/}"; \
	    header_file=$$(basename "$${path}"); \
	    header_dir=$$(dirname "$${path}"); \
	    abs_header_dir=$$(python -c "import os; print os.path.abspath(\"$${header_dir}\")"); \
	    install_header_file=$${abs_header_dir}/$${header_file}; \
	    install_header_dir=$${abs_header_dir}; \
	    printf -- "$(V_BEGIN_BLUE)lib$(TARGET)/$${pair_header_file#$(THIS_ROOT)/lib$(TARGET)/./}$(V_END)\n"; \
	    mkdir -p $${install_header_dir}; \
	    $(INSTALL) -m 644 $${pair_header_file} $${install_header_file}; \
	done
	$(ECHO)mkdir -p $(INSTALL_ROOT)/sh-elf/sh-elf/lib/ldscripts
	$(ECHO)for ldscript in $(LDSCRIPTS_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${ldscript#$(THIS_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$ldscript $(INSTALL_ROOT)/sh-elf/sh-elf/lib/ldscripts/; \
	done
	$(ECHO)for specs in $(SPECS_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${specs#$(THIS_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$specs $(INSTALL_ROOT)/sh-elf/sh-elf/lib/; \
	done
	$(ECHO)mkdir -p $(INSTALL_ROOT)/sh-elf/share/$(TARGET)/bootstrap
	$(ECHO)for bootstrap_file in $(BOOTSTRAP_FILES_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${bootstrap_file#$(THIS_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$bootstrap_file $(INSTALL_ROOT)/sh-elf/share/$(TARGET)/bootstrap; \
	done
	$(ECHO)mkdir -p $(INSTALL_ROOT)/share
	$(ECHO)for user_file in $(USER_FILES_all); do \
	    printf -- "$(V_BEGIN_BLUE)$${user_file#$(THIS_ROOT)/}$(V_END)\n"; \
	    $(INSTALL) -m 644 $$user_file $(INSTALL_ROOT)/share/; \
	done
endef

define macro-clean
	$(ECHO)if [ -d $(BUILD_ROOT)/$(SUB_BUILD)/$1 ]; then \
		$(FIND) $(BUILD_ROOT)/$(SUB_BUILD)/$1 -type f -name "*.[od]" -exec $(RM) {} \;; \
	fi
endef

.PHONY: \
	all \
	release \
	release-internal \
	debug \
	clean \
	clean-release \
	clean-release-internal \
	clean-debug \
	install-release \
	install-release-internal \
	install-debug

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
	$(ECHO)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal:
	$(ECHO)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/debug:
	$(ECHO)mkdir -p $@

$(BUILD_ROOT)/$(SUB_BUILD)/release/lib$(TARGET).a: $(LIB_OBJS_release)
	$(call macro-build-library,release)

$(BUILD_ROOT)/$(SUB_BUILD)/release-internal/lib$(TARGET).a: $(LIB_OBJS_release-internal)
	$(call macro-build-library,release-internal)

$(BUILD_ROOT)/$(SUB_BUILD)/debug/lib$(TARGET).a: $(LIB_OBJS_debug)
	$(call macro-build-library,debug)

$(BUILD_ROOT)/$(SUB_BUILD)/%.x: %.x
	@printf -- "$(V_BEGIN_YELLOW)$(shell v="$@"; printf -- "$${v#$(BUILD_ROOT)/}")$(V_END)\n"
	$(ECHO)mkdir -p $(@D)
	$(ECHO)cat $< | awk '/^SEARCH_DIR[[:space:]]+(.+);$$/ { \
	    sub(/\$$INSTALL_ROOT/,"$(INSTALL_ROOT)/sh-elf"); \
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
