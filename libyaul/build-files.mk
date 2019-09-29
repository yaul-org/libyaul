# -*- mode: makefile -*-

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
	common/update-cdb

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

ifneq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),0)
LIB_SRCS+= \
	common/gdb/gdb.c \
	common/gdb/gdb-ihr.sx
endif

LIB_SRCS+= \
	kernel/dbgio/dbgio.c \
	kernel/dbgio/devices/null.c \
	kernel/dbgio/devices/vdp1.c \
	kernel/dbgio/devices/vdp2.c \
	kernel/dbgio/devices/usb-cart.c \
	kernel/dbgio/devices/cons/cons.c \
	\
	kernel/sys/dma-queue.c \
	kernel/sys/callback-list.c \
	\
	kernel/mm/memb.c

ifeq ($(strip $(YAUL_OPTION_MALLOC_IMPL)),tlsf)
LIB_SRCS+= \
	kernel/mm/tlsf.c
endif

ifeq ($(strip $(YAUL_OPTION_MALLOC_IMPL)),slob)
LIB_SRCS+= \
	kernel/mm/slob.c
endif

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
	lib/stdlib/abs.c \
	lib/stdlib/assert.c \
	lib/stdlib/atoi.c \
	lib/stdlib/atol.c \
	lib/stdlib/free.c \
	lib/stdlib/malloc.c \
	lib/stdlib/realloc.c

LIB_SRCS+= \
	kernel/vfs/fs/romdisk/romdisk.c

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
	scu/bus/b/vdp/vdp_init.c \
	scu/bus/b/vdp/vdp_sync.c \
	scu/bus/b/vdp/vdp-internal.c \
	scu/bus/b/vdp/vdp1_cmdt.c \
	scu/bus/b/vdp/vdp1_env.c \
	scu/bus/b/vdp/vdp1_vram.c \
	scu/bus/b/vdp/vdp2_cram.c \
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
	scu/bus/cpu/cpu_ubc_init.c \
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

INCLUDE_DIRS:= \
	. \
	common \
	common/gdb \
	lib/lib \
	kernel \
	kernel/dbgio \
	kernel/vfs \
	math \
	scu \
	scu/bus/a/cs0/arp \
	scu/bus/a/cs0/dram-cart \
	scu/bus/a/cs0/usb-cart \
	scu/bus/a/cs2/cd-block \
	scu/bus/b/scsp \
	scu/bus/b/vdp \
	scu/bus/cpu \
	scu/bus/cpu/smpc

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
	./lib/lib/sys/:cdefs.h:./sys/ \
	./lib/lib/sys/:queue.h:./sys/ \
	./lib/lib/sys/:types.h:./sys/ \
	./kernel/sys/:init.h:./sys/

INSTALL_HEADER_FILES+= \
	./math/:color.h:yaul/math/ \
	./math/:fix.h:yaul/math/ \
	./math/:fix16.h:yaul/math/ \
	./math/:fract32.h:yaul/math/ \
	./math/:int16.h:yaul/math/ \
	./math/:int64.h:yaul/math/ \
	./math/:math.h:yaul/math/ \
	./math/:uint32.h:yaul/math/

INSTALL_HEADER_FILES+= \
	./common/bootstrap/:ip.h:yaul/common/ \
	./common/gdb/:gdb.h:yaul/common/gdb/

INSTALL_HEADER_FILES+= \
	./kernel/dbgio/:dbgio.h:yaul/dbgio/

INSTALL_HEADER_FILES+= \
	./kernel/mm/:memb.h:yaul/mm/

ifeq ($(strip $(YAUL_OPTION_MALLOC_IMPL)),tlsf)
INSTALL_HEADER_FILES+= \
	./kernel/mm/:tlsf.h:yaul/mm/
endif

ifeq ($(strip $(YAUL_OPTION_MALLOC_IMPL)),slob)
INSTALL_HEADER_FILES+= \
	./kernel/mm/:slob.h:yaul/mm/
endif

INSTALL_HEADER_FILES+= \
	./kernel/sys/:dma-queue.h:yaul/sys/

INSTALL_HEADER_FILES+= \
	./kernel/vfs/fs/romdisk/:romdisk.h:yaul/fs/romdisk/

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
	./scu/bus/b/vdp/vdp2/:sprite.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:tvmd.h:yaul/scu/bus/b/vdp/vdp2/ \
	./scu/bus/b/vdp/vdp2/:vram.h:yaul/scu/bus/b/vdp/vdp2/ \
	\
	./scu/bus/cpu/:cpu.h:yaul/scu/bus/cpu/ \
	./scu/bus/cpu/cpu/:cache.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:divu.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:dmac.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:endian.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:frt.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:instructions.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:intc.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:map.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:registers.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:dual.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:sync.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:ubc.h:yaul/scu/bus/cpu/cpu/ \
	./scu/bus/cpu/cpu/:wdt.h:yaul/scu/bus/cpu/cpu/ \
	\
	./scu/bus/cpu/smpc/:smpc.h:yaul/scu/bus/cpu/smpc/ \
	./scu/bus/cpu/smpc/smpc/:peripheral.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:map.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:rtc.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:smc.h:yaul/scu/bus/cpu/smpc/smpc/
