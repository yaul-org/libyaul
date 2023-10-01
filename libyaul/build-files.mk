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
	build/build.post.bin.mk \
	build/build.post.iso-cue.mk \
	build/build.post.ss.mk \
	build/build.pre.mk \
	build/ip.bin.mk \
	build/build.yaul.mk

HELPER_FILES:= \
	common/wrap-error

LDSCRIPTS:= \
	common/ldscripts/yaul.x \
	common/ldscripts/yaul-c++.x \
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
	kernel/reset-internal.c \
	kernel/internal.c \
	kernel/mm/internal.c

LIB_SRCS+= \
	bup/bup.c

LIB_SRCS+= \
	kernel/dbgio/dbgio.c \
	kernel/dbgio/devices/cons/cons.c \
	kernel/dbgio/devices/mednafen-debug.c \
	kernel/dbgio/devices/null.c \
	kernel/dbgio/devices/usb-cart.c \
	kernel/dbgio/devices/vdp1.c \
	kernel/dbgio/devices/vdp2.c \
	kernel/dbgio/font/default_font.c \
	kernel/dbgio/font/font.c

LIB_SRCS+= \
	kernel/sys/dma-queue.c \
	kernel/sys/dma-queue-internal.c \
	kernel/sys/callback-list.c \
	kernel/sys/callback-list-internal.c \
	\
	kernel/mm/arena.c \
	kernel/mm/memb.c \
	kernel/mm/memb-internal.c \
	kernel/mm/pagep.c \
	kernel/mm/mm_stats.c

# TLSF is required
LIB_SRCS+= \
	kernel/mm/tlsf.c

LIB_SRCS+= \
	libc/ctype/ctype.c

LIB_SRCS+= \
	libc/errno/strerror.c \
	libc/errno/__errno_location.c \

LIB_SRCS+= \
	libc/string/bcmp.c \
	libc/string/bcopy.c \
	libc/string/bzero.c \
	libc/string/index.c \
	libc/string/memccpy.c \
	libc/string/memchr.c \
	libc/string/memcmp.c \
	libc/string/memcpy.c \
	libc/string/memmem.c \
	libc/string/memmove.c \
	libc/string/mempcpy.c \
	libc/string/memrchr.c \
	libc/string/memset.c \
	libc/string/rindex.c \
	libc/string/stpcpy.c \
	libc/string/stpncpy.c \
	libc/string/strcasecmp.c \
	libc/string/strcasestr.c \
	libc/string/strcat.c \
	libc/string/strchr.c \
	libc/string/strchrnul.c \
	libc/string/strcmp.c \
	libc/string/strcpy.c \
	libc/string/strcspn.c \
	libc/string/strdup.c \
	libc/string/strlcat.c \
	libc/string/strlcpy.c \
	libc/string/strlen.c \
	libc/string/strncasecmp.c \
	libc/string/strncat.c \
	libc/string/strncmp.c \
	libc/string/strncpy.c \
	libc/string/strndup.c \
	libc/string/strnlen.c \
	libc/string/strpbrk.c \
	libc/string/strrchr.c \
	libc/string/strsep.c \
	libc/string/strspn.c \
	libc/string/strstr.c \
	libc/string/strtok.c \
	libc/string/strtok_r.c \
	libc/string/strverscmp.c \
	libc/string/swab.c

LIB_SRCS+= \
	libc/prng/prng_xorshift32.c \
	libc/prng/prng_xorwow.c \
	libc/prng/rand.c

LIB_SRCS+= \
	libc/stdio/clearerr.c \
	libc/stdio/stdio.c \
    libc/stdio/fclose.c \
    libc/stdio/feof.c \
    libc/stdio/ferror.c \
    libc/stdio/fflush.c \
    libc/stdio/fgetc.c \
    libc/stdio/fgetpos.c \
    libc/stdio/fgets.c \
    libc/stdio/fopen.c \
    libc/stdio/fprintf.c \
    libc/stdio/fputc.c \
    libc/stdio/fputs.c \
    libc/stdio/fread.c \
    libc/stdio/freopen.c \
    libc/stdio/fseek.c \
    libc/stdio/fsetpos.c \
    libc/stdio/ftell.c \
    libc/stdio/fwrite.c \
    libc/stdio/getc.c \
    libc/stdio/getchar.c \
    libc/stdio/gets.c \
    libc/stdio/perror.c \
    libc/stdio/printf.c \
    libc/stdio/putc.c \
    libc/stdio/putchar.c \
    libc/stdio/puts.c \
    libc/stdio/remove.c \
    libc/stdio/rename.c \
    libc/stdio/rewind.c \
    libc/stdio/setbuf.c \
    libc/stdio/setvbuf.c \
    libc/stdio/snprintf.c \
    libc/stdio/sprintf.c \
    libc/stdio/tmpfile.c \
    libc/stdio/tmpnam.c \
    libc/stdio/ungetc.c \
    libc/stdio/vfprintf.c \
    libc/stdio/vprintf.c \
    libc/stdio/vsnprintf.c \
    libc/stdio/vsprintf.c

LIB_SRCS+= \
	libc/exit/abort.c \
	libc/exit/assert.c \
	libc/exit/atexit.c \
	libc/exit/exit.c

LIB_SRCS+= \
	libc/stdlib/atoi.c \
	libc/stdlib/atol.c \
	libc/stdlib/free.c \
	libc/stdlib/malloc.c \
	libc/stdlib/memalign.c \
	libc/stdlib/realloc.c \
	libc/stdlib/strtol.c

LIB_SRCS+= \
	libc/crc/crc.c

LIB_SRCS+= \
	kernel/fs/cd/cdfs.c \
	kernel/fs/cd/cdfs_sector_read.c

LIB_SRCS+= \
	scu/bus/a/cs0/arp/arp.c

LIB_SRCS+= \
	scu/bus/a/cs0/dram-cart/dram-cart.c

LIB_SRCS+= \
	scu/bus/a/cs0/usb-cart/usb-cart.c

LIB_SRCS+= \
	gamemath/color/color.c \
\
	gamemath/fix16/fix16.c \
	gamemath/fix16/fix16_mat33.c \
	gamemath/fix16/fix16_mat43.c \
	gamemath/fix16/fix16_plane.c \
	gamemath/fix16/fix16_sqrt.c \
	gamemath/fix16/fix16_str.c \
	gamemath/fix16/fix16_trig.c \
	gamemath/fix16/fix16_vec2.c \
	gamemath/fix16/fix16_vec3.c \
\
	gamemath/fix16/c++/fix16_vec3.cxx \
\
	gamemath/int16.c \
	gamemath/int32.c \
	gamemath/uint16.c \
	gamemath/uint32.c \
	gamemath/math3d.c \
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
	scu/bus/b/vdp/vdp2_scrn_back.c \
	scu/bus/b/vdp/vdp2_scrn_bitmap.c \
	scu/bus/b/vdp/vdp2_scrn_cell.c \
	scu/bus/b/vdp/vdp2_scrn_color_offset.c \
	scu/bus/b/vdp/vdp2_scrn_display.c \
	scu/bus/b/vdp/vdp2_scrn_lncl.c \
	scu/bus/b/vdp/vdp2_scrn_ls.c \
	scu/bus/b/vdp/vdp2_scrn_mosaic.c \
	scu/bus/b/vdp/vdp2_scrn_priority.c \
	scu/bus/b/vdp/vdp2_scrn_reduction.c \
	scu/bus/b/vdp/vdp2_scrn_rotation.c \
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
	./intellisense.h \
	./ip/ip.h

INSTALL_HEADER_FILES:= \
	./:yaul.h:./

INSTALL_HEADER_FILES+= \
	./:intellisense.h:./

INSTALL_HEADER_FILES+= \
	./:bios.h:./yaul/

INSTALL_HEADER_FILES+= \
	./bup/:bup.h:./yaul/

INSTALL_HEADER_FILES+= \
	./libc/libc/bits/:alltypes.h:./bits/ \
	./libc/libc/bits/:fcntl.h:./bits/

INSTALL_HEADER_FILES+= \
	./libc/libc/sys/:cdefs.h:./sys/ \
	./libc/libc/sys/:queue.h:./sys/ \
	./libc/libc/sys/:types.h:./sys/ \
	./kernel/sys/:init.h:./sys/

INSTALL_HEADER_FILES+= \
	./libc/libc/:alloca.h:./ \
	./libc/libc/:assert.h:./ \
	./libc/libc/:ctype.h:./ \
	./libc/libc/:errno.h:./ \
	./libc/libc/:fcntl.h:./ \
	./libc/libc/:math.h:./ \
	./libc/libc/:stdio.h:./ \
	./libc/libc/:stdlib.h:./ \
	./libc/libc/:string.h:./ \
	./libc/libc/:unistd.h:./

INSTALL_HEADER_FILES+= \
	./libc/libc/:crc.h:./

INSTALL_HEADER_FILES+= \
	./libc/libc/:prng.h:./

INSTALL_HEADER_FILES+= \
	./gamemath/:gamemath.h:yaul/ \
\
	./gamemath/gamemath/:angle.h:yaul/gamemath/ \
	./gamemath/gamemath/:color.h:yaul/gamemath/ \
	./gamemath/gamemath/color/:hsv.h:yaul/gamemath/color/ \
	./gamemath/gamemath/color/:rgb1555.h:yaul/gamemath/color/ \
	./gamemath/gamemath/color/:rgb888.h:yaul/gamemath/color/ \
\
	./gamemath/gamemath/:fix16.h:yaul/gamemath/ \
	./gamemath/gamemath/fix16/:fix16_mat43.h:yaul/gamemath/fix16/ \
	./gamemath/gamemath/fix16/:fix16_mat33.h:yaul/gamemath/fix16/ \
	./gamemath/gamemath/fix16/:fix16_plane.h:yaul/gamemath/fix16/ \
	./gamemath/gamemath/fix16/:fix16_trig.h:yaul/gamemath/fix16/ \
	./gamemath/gamemath/fix16/:fix16_vec2.h:yaul/gamemath/fix16/ \
	./gamemath/gamemath/fix16/:fix16_vec3.h:yaul/gamemath/fix16/ \
\
	./gamemath/gamemath/:defs.h:yaul/gamemath/ \
	./gamemath/gamemath/:int8.h:yaul/gamemath/ \
	./gamemath/gamemath/:int16.h:yaul/gamemath/ \
	./gamemath/gamemath/:int32.h:yaul/gamemath/ \
	./gamemath/gamemath/:math3d.h:yaul/gamemath/ \
	./gamemath/gamemath/:uint8.h:yaul/gamemath/ \
	./gamemath/gamemath/:uint16.h:yaul/gamemath/ \
	./gamemath/gamemath/:uint32.h:yaul/gamemath/

INSTALL_HEADER_FILES+= \
	./ip/:ip.h:yaul/

INSTALL_HEADER_FILES+= \
	./kernel/dbgio/:dbgio.h:yaul/

INSTALL_HEADER_FILES+= \
	./kernel/mm/:arena.h:yaul/mm/ \
	./kernel/mm/:memb.h:yaul/mm/ \
	./kernel/mm/:pagep.h:yaul/mm/ \
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
	./scu/:scu.h:yaul/ \
	./scu/scu/:dma.h:yaul/scu/ \
	./scu/scu/:dsp.h:yaul/scu/ \
	./scu/scu/:ic.h:yaul/scu/ \
	./scu/scu/:map.h:yaul/scu/ \
	./scu/scu/:timer.h:yaul/scu/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs2/cd-block/:cd-block.h:yaul/ \
	./scu/bus/a/cs2/cd-block/cd-block/:cmd.h:yaul/cd-block/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/dram-cart/:dram-cart.h:yaul/ \
	./scu/bus/a/cs0/dram-cart/:dram-cart/map.h:yaul/dram-cart/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/flash/:flash.h:yaul/ \
	./scu/bus/a/cs0/flash/flash/:map.h:yaul/flash/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/arp/:arp.h:yaul/ \
	./scu/bus/a/cs0/arp/arp/:map.h:yaul/arp/

INSTALL_HEADER_FILES+= \
	./scu/bus/a/cs0/usb-cart/:usb-cart.h:yaul/ \
	./scu/bus/a/cs0/usb-cart/usb-cart/:map.h:yaul/usb-cart/

INSTALL_HEADER_FILES+= \
	./scu/bus/b/scsp/:scsp.h:yaul/

INSTALL_HEADER_FILES+= \
	./scu/bus/b/vdp/:vdp.h:yaul/ \
	./scu/bus/b/vdp/:vdp1.h:yaul/ \
	./scu/bus/b/vdp/vdp1/:cmdt.h:yaul/vdp1/ \
	./scu/bus/b/vdp/vdp1/:env.h:yaul/vdp1/ \
	./scu/bus/b/vdp/vdp1/:map.h:yaul/vdp1/ \
	./scu/bus/b/vdp/vdp1/:vram.h:yaul/vdp1/ \
	./scu/bus/b/vdp/:vdp2.h:yaul/ \
	./scu/bus/b/vdp/vdp2/:cram.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:map.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_bitmap.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_cell.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_funcs.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_macros.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_rotation.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:scrn_shared.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:sprite.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:tvmd.h:yaul/vdp2/ \
	./scu/bus/b/vdp/vdp2/:vram.h:yaul/vdp2/

INSTALL_HEADER_FILES+= \
	./scu/bus/cpu/:cpu.h:yaul/ \
	./scu/bus/cpu/cpu/:cache.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:divu.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:dmac.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:dual.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:endian.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:frt.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:instructions.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:intc.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:map.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:registers.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:sci.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:sync.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:ubc.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:wdt.h:yaul/cpu/ \
	./scu/bus/cpu/cpu/:which.h:yaul/cpu/

INSTALL_HEADER_FILES+= \
	./scu/bus/cpu/smpc/:smpc.h:yaul/ \
	./scu/bus/cpu/smpc/smpc/:peripheral.h:yaul/smpc/ \
	./scu/bus/cpu/smpc/smpc/:map.h:yaul/smpc/ \
	./scu/bus/cpu/smpc/smpc/:rtc.h:yaul/smpc/ \
	./scu/bus/cpu/smpc/smpc/:smc.h:yaul/smpc/
