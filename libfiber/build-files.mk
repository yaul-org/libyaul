# -*- mode: makefile -*-

LIB_SRCS:= \
	fiber.c \
	context_switch.sx

INSTALL_HEADER_FILES:= \
	./:fiber.h:./

USER_FILES:= \
	build/build.fiber.mk
