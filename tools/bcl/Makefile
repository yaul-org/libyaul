include ../../env.mk

.PHONY: all clean distclean install

all:
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_huffman SRCS="huffman.c shared.c" -f bcl_prog.mk
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_lz SRCS="lz.c shared.c" -f bcl_prog.mk
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_prs SRCS="prs.c shared.c" -f bcl_prog.mk
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_rle SRCS="rle.c shared.c" -f bcl_prog.mk

clean:
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_huffman SRCS="huffman.c shared.c" -f bcl_prog.mk clean
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_lz SRCS="lz.c shared.c" -f bcl_prog.mk clean
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_prs SRCS="prs.c shared.c" -f bcl_prog.mk clean
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_rle SRCS="rle.c shared.c" -f bcl_prog.mk clean

distclean: clean

install: all
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_huffman SRCS="huffman.c shared.c" -f bcl_prog.mk install
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_lz SRCS="lz.c shared.c" -f bcl_prog.mk install
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_prs SRCS="prs.c shared.c" -f bcl_prog.mk install
	$(ECHO)$(MAKE) --no-print-directory TARGET=bcl_rle SRCS="rle.c shared.c" -f bcl_prog.mk install
