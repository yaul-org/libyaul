SET(SRC tga.c)

SET(INCLUDE_DIRS
  ../libyaul/
  ../libyaul/common
  ../libyaul/common/gdb
  ../libyaul/cons
  ../libyaul/kernel
  ../libyaul/scu
  ../libyaul/scu/bus/a/cs0/dram-cartridge
  ../libyaul/scu/bus/a/cs0/usb-cartridge
  ../libyaul/scu/bus/a/cs2/cd-block
  ../libyaul/scu/bus/b/scsp
  ../libyaul/scu/bus/b/vdp1
  ../libyaul/scu/bus/b/vdp2
  ../libyaul/scu/bus/cpu
  ../libyaul/scu/bus/cpu/smpc
)


INCLUDE_DIRECTORIES(AFTER ${INCLUDE_DIRS})

ADD_LIBRARY(tga STATIC ${SRC})
