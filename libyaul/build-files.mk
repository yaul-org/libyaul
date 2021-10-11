# -*- mode: makefile -*-

FLAG_BUILD_GDB:= $(shell test "x$(YAUL_OPTION_DEV_CARTRIDGE)" != x0 -a \
                              \( "x$(TYPE)" = xdebug -o \
                                 "x$(YAUL_OPTION_BUILD_GDB)" != x0 \) && printf "yes")

BOOTSTRAP_FILES:= \
	common/bootstrap/ip.sx \
	common/bootstrap/sys_aree.bin \
	common/bootstrap/sys_arej.bin \
	common/bootstrap/sys_aret.bin \
	common/bootstrap/sys_areu.bin \
	common/bootstrap/sys_init.bin \
	common/bootstrap/sys_sec.bin

USER_FILES:= \
	common/pre.common.mk \
	common/post.common.mk

HELPER_FILES:= \
	common/update-cdb \
	common/wrap-error

LDSCRIPTS:= \
	common/ldscripts/yaul.x \
	common/ldscripts/ip.x

SPECS:= \
	common/specs/yaul.specs \
	common/specs/yaul-main.specs \
	common/specs/ip.specs

SUPPORT_SRCS:= \
	common/crt0.sx \
	common/c++-support.cxx \

# Keep this in SUPPORT_SRCS as the object file is copied to the tool-chain
SUPPORT_SRCS+= \
	kernel/sys/init.c

LIB_SRCS:= \
	kernel/internal.c \
	kernel/mm/internal.c \
	common/internal_reset.c

ifeq ($(strip $(FLAG_BUILD_GDB)),yes)
LIB_SRCS+= \
	common/gdb/gdb.c \
	common/gdb/gdb-ihr.sx
endif

LIB_SRCS+= \
	kernel/dbgio/dbgio.c \
	kernel/dbgio/devices/null.c \
	kernel/dbgio/devices/vdp1.c \
	kernel/dbgio/devices/vdp2.c \

ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),1)
LIB_SRCS+= \
	kernel/dbgio/devices/usb-cart.c
endif

LIB_SRCS+= \
	kernel/dbgio/devices/cons/cons.c \
	\
	kernel/sys/dma-queue.c \
	kernel/sys/callback-list.c \
	\
	kernel/mm/memb.c

# TLSF is required
LIB_SRCS+= \
	kernel/mm/tlsf.c

LIB_SRCS+= \
	lib/ctype/ctype.c \
	lib/string/bcmp.c \
	lib/string/bcopy.c \
	lib/string/bzero.c \
	lib/string/index.c \
	lib/string/memccpy.c \
	lib/string/memchr.c \
	lib/string/memcmp.c \
	lib/string/memcpy.c \
	lib/string/memmem.c \
	lib/string/memmove.c \
	lib/string/mempcpy.c \
	lib/string/memrchr.c \
	lib/string/memset.c \
	lib/string/rindex.c \
	lib/string/stpcpy.c \
	lib/string/stpncpy.c \
	lib/string/strcasecmp.c \
	lib/string/strcasestr.c \
	lib/string/strcat.c \
	lib/string/strchr.c \
	lib/string/strchrnul.c \
	lib/string/strcmp.c \
	lib/string/strcpy.c \
	lib/string/strcspn.c \
	lib/string/strdup.c \
	lib/string/strlcat.c \
	lib/string/strlcpy.c \
	lib/string/strlen.c \
	lib/string/strncasecmp.c \
	lib/string/strncat.c \
	lib/string/strncmp.c \
	lib/string/strncpy.c \
	lib/string/strndup.c \
	lib/string/strnlen.c \
	lib/string/strpbrk.c \
	lib/string/strrchr.c \
	lib/string/strsep.c \
	lib/string/strspn.c \
	lib/string/strstr.c \
	lib/string/strtok.c \
	lib/string/strtok_r.c \
	lib/string/strverscmp.c \
	lib/string/swab.c \
	lib/stdio/snprintf.c \
	lib/stdlib/abort.c \
	lib/stdlib/abs.c

ifeq ($(strip $(YAUL_OPTION_BUILD_ASSERT)),1)
LIB_SRCS+= \
	lib/stdlib/assert.c
endif

LIB_SRCS+= \
	lib/stdlib/atoi.c \
	lib/stdlib/atol.c \
	lib/stdlib/free.c \
	lib/stdlib/labs.c \
	lib/stdlib/malloc.c \
	lib/stdlib/memalign.c \
	lib/stdlib/realloc.c \
	lib/stdlib/strtol.c

LIB_SRCS+= \
	lib/crc/crc.c

LIB_SRCS+= \
	kernel/vfs/fs/romdisk/romdisk.c

LIB_SRCS+= \
	kernel/vfs/fs/iso9660/iso9660.c

ifneq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),0)
LIB_SRCS+= \
	kernel/vfs/fs/fileclient/fileclient.c
endif

ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),2)
LIB_SRCS+= \
	scu/bus/a/cs0/arp/arp.c
endif

LIB_SRCS+= \
	scu/bus/a/cs0/dram-cart/dram-cart.c

ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),1)
LIB_SRCS+= \
	scu/bus/a/cs0/usb-cart/usb-cart.c
endif

LIB_SRCS+= \
	math/color.c \
	math/fix16.c \
	math/fix16_mat3.c \
	math/fix16_plane.c \
	math/fix16_sqrt.c \
	math/fix16_str.c \
	math/fix16_trig.c \
	math/fix16_vec2.c \
	math/fix16_vec3.c \
	math/int16.c \
	math/uint32.c \
	scu/bus/a/cs2/cd-block/cd-block_cmds.c \
	scu/bus/a/cs2/cd-block/cd-block_execute.c \
	scu/bus/a/cs2/cd-block/cd-block_init.c \
	\
	scu/bus/b/scsp/scsp_init.c \
	\
	scu/bus/b/vdp/vdp_init.c \
	scu/bus/b/vdp/vdp_sync.c \
	scu/bus/b/vdp/vdp-internal.c \
	scu/bus/b/vdp/vdp1_cmdt.c \
	scu/bus/b/vdp/vdp1_env.c \
	scu/bus/b/vdp/vdp1_vram.c \
	scu/bus/b/vdp/vdp2_cram.c \
	scu/bus/b/vdp/vdp2_regs.c \
	scu/bus/b/vdp/vdp2_scrn.c \
	scu/bus/b/vdp/vdp2_scrn_back_screen.c \
	scu/bus/b/vdp/vdp2_scrn_color_offset.c \
	scu/bus/b/vdp/vdp2_scrn_display.c \
	scu/bus/b/vdp/vdp2_scrn_ls.c \
	scu/bus/b/vdp/vdp2_scrn_mosaic.c \
	scu/bus/b/vdp/vdp2_scrn_priority.c \
	scu/bus/b/vdp/vdp2_scrn_reduction.c \
	scu/bus/b/vdp/vdp2_scrn_scroll.c \
	scu/bus/b/vdp/vdp2_scrn_sf.c \
	scu/bus/b/vdp/vdp2_scrn_vcs.c \
	scu/bus/b/vdp/vdp2_sprite.c \
	scu/bus/b/vdp/vdp2_tvmd.c \
	scu/bus/b/vdp/vdp2_vram.c \
	\
	scu/bus/cpu/cpu_cache.c \
	scu/bus/cpu/cpu_divu.c \
	scu/bus/cpu/cpu_dmac.c \
	scu/bus/cpu/cpu_dual.c \
	scu/bus/cpu/cpu_frt.c \
	scu/bus/cpu/cpu_init.c \
	scu/bus/cpu/cpu_sci.c \
	scu/bus/cpu/cpu_ubc.c \
	scu/bus/cpu/cpu_wdt.c \
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
	\
	scu/scu-internal.c \
	scu/scu_init.c \
	scu/scu_dma.c \
	scu/scu_dsp.c \
	scu/scu_timer.c

HEADER_FILES:= \
	./yaul.h \
	./common/bootstrap/ip.h

INSTALL_HEADER_FILES:= \
	./:yaul.h:./yaul/

INSTALL_HEADER_FILES+= \
	./:bios.h:./yaul/

INSTALL_HEADER_FILES+= \
	./lib/lib/:alloca.h:./ \
	./lib/lib/:assert.h:./ \
	./lib/lib/:ctype.h:./ \
	./lib/lib/:errno.h:./ \
	./lib/lib/:stdio.h:./ \
	./lib/lib/:stdlib.h:./ \
	./lib/lib/:string.h:./ \

INSTALL_HEADER_FILES+= \
	./lib/lib/sys/:cdefs.h:./sys/ \
	./lib/lib/sys/:queue.h:./sys/ \
	./lib/lib/sys/:types.h:./sys/ \
	./kernel/sys/:init.h:./sys/

INSTALL_HEADER_FILES+= \
	./lib/lib/:crc.h:./ \

INSTALL_HEADER_FILES+= \
	./math/:color.h:yaul/math/ \
	./math/:fix.h:yaul/math/ \
	./math/:fix16.h:yaul/math/ \
	./math/:fix16_mat3.h:yaul/math/ \
	./math/:fix16_plane.h:yaul/math/ \
	./math/:fix16_trig.h:yaul/math/ \
	./math/:fix16_vec2.h:yaul/math/ \
	./math/:fix16_vec3.h:yaul/math/ \
	./math/:int16.h:yaul/math/ \
	./math/:int32.h:yaul/math/ \
	./math/:int8.h:yaul/math/ \
	./math/:math.h:yaul/math/ \
	./math/:uint32.h:yaul/math/ \
	./math/:uint8.h:yaul/math/

INSTALL_HEADER_FILES+= \
	./common/bootstrap/:ip.h:yaul/common/

ifeq ($(strip $(FLAG_BUILD_GDB)),yes)
INSTALL_HEADER_FILES+= \
	./common/gdb/:gdb.h:yaul/common/gdb/
endif

INSTALL_HEADER_FILES+= \
	./kernel/dbgio/:dbgio.h:yaul/dbgio/

INSTALL_HEADER_FILES+= \
	./kernel/mm/:memb.h:yaul/mm/

# TLSF is required
INSTALL_HEADER_FILES+= \
	./kernel/mm/:tlsf.h:yaul/mm/

INSTALL_HEADER_FILES+= \
	./kernel/sys/:dma-queue.h:yaul/sys/

INSTALL_HEADER_FILES+= \
	./kernel/sys/:callback-list.h:yaul/sys/

INSTALL_HEADER_FILES+= \
	./kernel/vfs/fs/romdisk/:romdisk.h:yaul/fs/romdisk/

INSTALL_HEADER_FILES+= \
	./kernel/vfs/fs/iso9660/:iso9660.h:yaul/fs/iso9660/

ifneq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),0)
INSTALL_HEADER_FILES+= \
	./kernel/vfs/fs/fileclient/:fileclient.h:yaul/fs/fileclient/
endif

INSTALL_HEADER_FILES+= \
	./scu/:scu.h:yaul/scu/ \
	./scu/scu/:dma.h:yaul/scu/scu/ \
	./scu/scu/:dsp.h:yaul/scu/scu/ \
	./scu/scu/:ic.h:yaul/scu/scu/ \
	./scu/scu/:map.h:yaul/scu/scu/ \
	./scu/scu/:timer.h:yaul/scu/scu/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs2/cd-block/:cd-block.h:yaul/scu/bus/a/cs2/cd-block/ \
	./scu/bus/a/cs2/cd-block/cd-block/:cmd.h:yaul/scu/bus/a/cs2/cd-block/cd-block/ \
	\
	./scu/bus/a/cs0/dram-cart/:dram-cart.h:yaul/scu/bus/a/cs0/dram-cart/ \
	./scu/bus/a/cs0/dram-cart/:dram-cart/map.h:yaul/scu/bus/a/cs0/dram-cart/

ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),2)
INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/arp/:arp.h:yaul/scu/bus/a/cs0/arp/ \
	./scu/bus/a/cs0/arp/arp/:map.h:yaul/scu/bus/a/cs0/arp/arp/
endif

ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),1)
INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/usb-cart/:usb-cart.h:yaul/scu/bus/a/cs0/usb-cart/ \
	./scu/bus/a/cs0/usb-cart/usb-cart/:map.h:yaul/scu/bus/a/cs0/usb-cart/usb-cart/
endif

INSTALL_HEADER_FILES+= \
	./scu/bus/b/scsp/:scsp.h:yaul/scu/bus/b/scsp/ \
	\
	./scu/bus/b/vdp/:vdp.h:yaul/scu/bus/b/vdp/ \
	./scu/bus/b/vdp/:vdp1.h:yaul/scu/bus/b/vdp/ \
	./scu/bus/b/vdp/vdp1/:cmdt.h:yaul/scu/bus/b/vdp/vdp1/ \
	./scu/bus/b/vdp/vdp1/:env.h:yaul/scu/bus/b/vdp/vdp1/ \
	./scu/bus/b/vdp/vdp1/:map.h:yaul/scu/bus/b/vdp/vdp1/ \
	./scu/bus/b/vdp/vdp1/:vram.h:yaul/scu/bus/b/vdp/vdp1/ \
	./scu/bus/b/vdp/:vdp2.h:yaul/scu/bus/b/vdp/ \
	./scu/bus/b/vdp/vdp2/:cram.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:map.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_macros.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:sprite.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:tvmd.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:vram.h:yaul/scu/bus/b/vdp/vdp2/ \
	\
	./scu/bus/cpu/:cpu.h:yaul/scu/bus/cpu/ \
	./scu/bus/cpu/cpu/:cache.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:divu.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:dmac.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:dual.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:endian.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:frt.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:instructions.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:intc.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:map.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:registers.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:sci.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:sync.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:ubc.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:wdt.h:yaul/scu/bus/cpu/cpu/ \
	\
	./scu/bus/cpu/smpc/:smpc.h:yaul/scu/bus/cpu/smpc/ \
	./scu/bus/cpu/smpc/smpc/:peripheral.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:map.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:rtc.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:smc.h:yaul/scu/bus/cpu/smpc/smpc/
