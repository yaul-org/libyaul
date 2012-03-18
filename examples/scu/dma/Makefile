PROJECT= dma
AS= sh-elf-as
AFLAGS+= --fatal-warnings --isa=sh2 --big --reduce-memory-overheads
CC= sh-elf-gcc
CFLAGS= -nostdlib -ansi -W -Wall -Wno-unused -m2 -mb -O2 -ffast-math -fomit-frame-pointer \
	-I../../../libyaul/common \
	-I../../../libyaul/scu \
	-I../../../libyaul/scu/bus/cpu \
	-I../../../libyaul/scu/bus/b/vdp1 \
	-I../../../libyaul/scu/bus/b/vdp2
LD= sh-elf-gcc
LDFLAGS+= -Wl,-Map,${PROJECT}.map -nostartfiles  -L.
OB= sh-elf-objcopy

OBJECTS= ../../../libyaul/common/crt0.o ../../../libyaul/common/crt0-init.o dma.o

.SUFFIXES: .c .o

.PHONY: clean

.c.o:
	${CC} ${CFLAGS} -c -o ${@} ${<}
.s:
	${AS} ${AFLAGS} -o ${@} ${<}

${PROJECT}.elf:${OBJECTS}
	${LD} ${LDFLAGS} -T ../../../libyaul/common/ldscripts/shelf.x ${?} -o ${@} -L../../../build/libyaul -lyaul
	$(OB) -O binary ${@} ${PROJECT}.bin

clean:
	-rm -f ${OBJECTS} ${PROJECT}.bin ${PROJECT}.elf ${PROJECT}.map
