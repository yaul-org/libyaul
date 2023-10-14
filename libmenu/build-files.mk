# -*- mode: makefile -*-

LIB_SRCS:= \
	menu.c \
	scroll_menu.c

INSTALL_HEADER_FILES:= \
	./:menu.h:./ \
	./:scroll_menu.h:./

USER_FILES:= \
	build/build.menu.mk
