# -*- mode: makefile -*-

include ../env.mk

TARGET:= bcl
TYPE:= debug
SUB_BUILD:=$(BUILD)/lib$(TARGET)

include build-files.mk
include base.mk
