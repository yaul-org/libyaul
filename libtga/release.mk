# -*- mode: makefile -*-

include ../env.mk

TARGET:= tga
TYPE:= release
SUB_BUILD:= $(BUILD)/lib$(TARGET)

include build-files.mk
include base.mk
