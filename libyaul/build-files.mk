# -*- mode: makefile -*-

IP_FILES:= \
	ip/ip.sx \
	ip/blobs/sys_aree.bin \
	ip/blobs/sys_arej.bin \
	ip/blobs/sys_aret.bin \
	ip/blobs/sys_areu.bin \
	ip/blobs/sys_init.bin \
	ip/blobs/sys_sec.bin

USER_FILES:= \
	common/build.pre.mk \
	common/build.post.bin.mk \
	common/build.post.iso-cue.mk \
	common/build.post.ss.mk \
	common/common.ip.bin.mk

HELPER_FILES:= \
	common/update-cdb \
	common/wrap-error

LDSCRIPTS:= \
	common/ldscripts/yaul.x \
	common/ldscripts/ip.x

SPECS:= \
	common/specs/yaul.specs \
	common/specs/yaul-main.specs \
	common/specs/yaul-main-c++.specs \
	common/specs/ip.specs

SUPPORT_SRCS:= \
	common/crt0.sx \
	common/c++-support.cxx \

# Keep this in SUPPORT_SRCS as the object file is copied to the tool-chain
SUPPORT_SRCS+= \
	kernel/sys/init.c

LIB_SRCS:= \
	kernel/ssload.c \
	kernel/internal.c \
	kernel/mm/internal.c \
	common/reset-internal.c

LIB_SRCS+= \
	kernel/dbgio/dbgio.c \
	kernel/dbgio/devices/null.c \
	kernel/dbgio/devices/vdp1.c \
	kernel/dbgio/devices/vdp2.c \

LIB_SRCS+= \
	kernel/dbgio/devices/usb-cart.c

LIB_SRCS+= \
	kernel/dbgio/devices/cons/cons.c \
	\
	kernel/sys/dma-queue.c \
	kernel/sys/dma-queue-internal.c \
	kernel/sys/callback-list.c \
	kernel/sys/callback-list-internal.c \
	\
	kernel/mm/memb.c \
	kernel/mm/memb-internal.c \
	kernel/mm/mm_stats.c

# TLSF is required
LIB_SRCS+= \
	kernel/mm/tlsf.c

LIB_SRCS+= \
	lib/ctype/ctype.c

LIB_SRCS+= \
	lib/errno/strerror.c \
	lib/errno/__errno_location.c \

LIB_SRCS+= \
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
	lib/string/swab.c

LIB_SRCS+= \
	lib/prng/prng_xorshift32.c \
	lib/prng/prng_xorwow.c \
	lib/prng/rand.c

LIB_SRCS+= \
	lib/stdio/clearerr.c \
	lib/stdio/stdio.c \
    lib/stdio/fclose.c \
    lib/stdio/feof.c \
    lib/stdio/ferror.c \
    lib/stdio/fflush.c \
    lib/stdio/fgetc.c \
    lib/stdio/fgetpos.c \
    lib/stdio/fgets.c \
    lib/stdio/fopen.c \
    lib/stdio/fprintf.c \
    lib/stdio/fputc.c \
    lib/stdio/fputs.c \
    lib/stdio/fread.c \
    lib/stdio/freopen.c \
    lib/stdio/fseek.c \
    lib/stdio/fsetpos.c \
    lib/stdio/ftell.c \
    lib/stdio/fwrite.c \
    lib/stdio/getc.c \
    lib/stdio/getchar.c \
    lib/stdio/gets.c \
    lib/stdio/perror.c \
    lib/stdio/printf.c \
    lib/stdio/putc.c \
    lib/stdio/putchar.c \
    lib/stdio/puts.c \
    lib/stdio/remove.c \
    lib/stdio/rename.c \
    lib/stdio/rewind.c \
    lib/stdio/setbuf.c \
    lib/stdio/setvbuf.c \
    lib/stdio/snprintf.c \
    lib/stdio/sprintf.c \
    lib/stdio/tmpfile.c \
    lib/stdio/tmpnam.c \
    lib/stdio/ungetc.c \
    lib/stdio/vfprintf.c \
    lib/stdio/vprintf.c \
    lib/stdio/vsnprintf.c \
    lib/stdio/vsprintf.c

LIB_SRCS+= \
	lib/exit/abort.c \
	lib/exit/assert.c \
	lib/exit/atexit.c \
	lib/exit/exit.c

LIB_SRCS+= \
	lib/stdlib/abs.c \
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
	kernel/fs/cd/cdfs.c \
	kernel/fs/cd/cdfs_sector_read.c

LIB_SRCS+= \
	kernel/fs/cd/cdfs_sector_usb_cart_read.c

LIB_SRCS+= \
	scu/bus/a/cs0/arp/arp.c

LIB_SRCS+= \
	scu/bus/a/cs0/dram-cart/dram-cart.c

LIB_SRCS+= \
	scu/bus/a/cs0/usb-cart/usb-cart.c

LIB_SRCS+= \
	math/color/color.c \
\
	math/fix16/fix16.c \
	math/fix16/fix16_mat3.c \
	math/fix16/fix16_plane.c \
	math/fix16/fix16_sqrt.c \
	math/fix16/fix16_str.c \
	math/fix16/fix16_trig.c \
	math/fix16/fix16_vec2.c \
	math/fix16/fix16_vec3.c \
\
	math/int16.c \
	math/int32.c \
	math/uint32.c \
\
	scu/bus/a/cs2/cd-block/cd-block_cmds.c \
	scu/bus/a/cs2/cd-block/cd-block_execute.c \
	scu/bus/a/cs2/cd-block/cd-block_init.c \
	\
	scu/bus/b/scsp/scsp_init.c \
	\
	scu/bus/b/vdp/vdp-internal.c \
	scu/bus/b/vdp/vdp1_cmdt.c \
	scu/bus/b/vdp/vdp1_env.c \
	scu/bus/b/vdp/vdp1_vram.c \
	scu/bus/b/vdp/vdp2_cram.c \
	scu/bus/b/vdp/vdp2_regs.c \
	scu/bus/b/vdp/vdp2_scrn.c \
	scu/bus/b/vdp/vdp2_scrn_back.c \
	scu/bus/b/vdp/vdp2_scrn_color_offset.c \
	scu/bus/b/vdp/vdp2_scrn_display.c \
	scu/bus/b/vdp/vdp2_scrn_lncl.c \
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
	scu/bus/b/vdp/vdp_init.c \
	scu/bus/b/vdp/vdp_sync.c \
	\
	scu/bus/cpu/cpu_cache.c \
	scu/bus/cpu/cpu_divu.c \
	scu/bus/cpu/cpu_dmac.c \
	scu/bus/cpu/cpu_dual.c \
	scu/bus/cpu/cpu_dual_entries.sx \
	scu/bus/cpu/cpu_exceptions.sx \
	scu/bus/cpu/cpu_frt.c \
	scu/bus/cpu/cpu_init.c \
	scu/bus/cpu/cpu_sci.c \
	scu/bus/cpu/cpu_ubc.c \
	scu/bus/cpu/cpu_wdt.c \
	\
	scu/bus/cpu/smpc/smpc_init.c \
	scu/bus/cpu/smpc/smpc_peripheral.c \
	scu/bus/cpu/smpc/smpc_peripherals.c \
	scu/bus/cpu/smpc/smpc_rtc.c \
	\
	scu/scu-internal.c \
	scu/scu_init.c \
	scu/scu_dma.c \
	scu/scu_dsp.c \
	scu/scu_timer.c

HEADER_FILES:= \
	./yaul.h \
	./ip/ip.h

INSTALL_HEADER_FILES:= \
	./:yaul.h:./yaul/

INSTALL_HEADER_FILES+= \
	./:bios.h:./yaul/

INSTALL_HEADER_FILES+= \
	./lib/lib/bits/:alltypes.h:./bits/ \
	./lib/lib/bits/:fcntl.h:./bits/ \

INSTALL_HEADER_FILES+= \
	./lib/lib/sys/:cdefs.h:./sys/ \
	./lib/lib/sys/:queue.h:./sys/ \
	./lib/lib/sys/:types.h:./sys/ \
	./kernel/sys/:init.h:./sys/

INSTALL_HEADER_FILES+= \
	./lib/lib/:alloca.h:./ \
	./lib/lib/:assert.h:./ \
	./lib/lib/:ctype.h:./ \
	./lib/lib/:errno.h:./ \
	./lib/lib/:fcntl.h:./ \
	./lib/lib/:stdio.h:./ \
	./lib/lib/:stdlib.h:./ \
	./lib/lib/:string.h:./ \
	./lib/lib/:unistd.h:./ \

INSTALL_HEADER_FILES+= \
	./lib/lib/:crc.h:./ \

INSTALL_HEADER_FILES+= \
	./lib/lib/:prng.h:./ \

INSTALL_HEADER_FILES+= \
	./math/:math.h:yaul/math/ \
\
	./math/:color.h:yaul/math/ \
	./math/color/:hsv.h:yaul/math/color/ \
	./math/color/:rgb1555.h:yaul/math/color/ \
	./math/color/:rgb888.h:yaul/math/color/ \
\
	./math/:fix16.h:yaul/math/ \
	./math/fix16/:fix16_mat3.h:yaul/math/fix16/ \
	./math/fix16/:fix16_plane.h:yaul/math/fix16/ \
	./math/fix16/:fix16_trig.h:yaul/math/fix16/ \
	./math/fix16/:fix16_vec2.h:yaul/math/fix16/ \
	./math/fix16/:fix16_vec3.h:yaul/math/fix16/ \
\
	./math/:int8.h:yaul/math/ \
	./math/:int16.h:yaul/math/ \
	./math/:int32.h:yaul/math/ \
	./math/:uint8.h:yaul/math/ \
	./math/:uint32.h:yaul/math/

INSTALL_HEADER_FILES+= \
	./ip/:ip.h:yaul/ip/

INSTALL_HEADER_FILES+= \
	./kernel/dbgio/:dbgio.h:yaul/dbgio/

INSTALL_HEADER_FILES+= \
	./kernel/mm/:memb.h:yaul/mm/ \
	./kernel/mm/:mm_stats.h:yaul/mm/

ifeq ($(strip $(YAUL_OPTION_MALLOC_IMPL)),tlsf)
INSTALL_HEADER_FILES+= \
	./kernel/mm/:tlsf.h:yaul/mm/
endif

INSTALL_HEADER_FILES+= \
	./kernel/sys/:dma-queue.h:yaul/sys/

INSTALL_HEADER_FILES+= \
	./kernel/sys/:callback-list.h:yaul/sys/

INSTALL_HEADER_FILES+= \
	./kernel/fs/cd/:cdfs.h:yaul/fs/cd/

INSTALL_HEADER_FILES+= \
	./kernel/:ssload.h:yaul/

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

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/flash/:flash.h:yaul/scu/bus/a/cs0/flash/ \
	./scu/bus/a/cs0/flash/flash/:map.h:yaul/scu/bus/a/cs0/flash/flash/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/arp/:arp.h:yaul/scu/bus/a/cs0/arp/ \
	./scu/bus/a/cs0/arp/arp/:map.h:yaul/scu/bus/a/cs0/arp/arp/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/usb-cart/:usb-cart.h:yaul/scu/bus/a/cs0/usb-cart/ \
	./scu/bus/a/cs0/usb-cart/usb-cart/:map.h:yaul/scu/bus/a/cs0/usb-cart/usb-cart/

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
	./scu/bus/cpu/cpu/:which.h:yaul/scu/bus/cpu/cpu/ \
	\
	./scu/bus/cpu/smpc/:smpc.h:yaul/scu/bus/cpu/smpc/ \
	./scu/bus/cpu/smpc/smpc/:peripheral.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:map.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:rtc.h:yaul/scu/bus/cpu/smpc/smpc/ \
	./scu/bus/cpu/smpc/smpc/:smc.h:yaul/scu/bus/cpu/smpc/smpc/
