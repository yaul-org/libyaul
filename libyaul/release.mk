# -*- mode: makefile -*-

include ../env.mk

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
endif

TARGET:= yaul
TYPE:= release
SUB_BUILD:=$(BUILD)/lib$(TARGET)

include build-files.mk
include base.mk
