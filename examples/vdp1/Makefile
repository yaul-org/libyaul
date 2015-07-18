include ../common.mk

PROJECT:= vdp1
OBJECTS+= \
	root.o \
	test.o \
	fs.o \
	state_machine.o \
	state.o \
	globals.o \
	vdp1.o \
	tests/00.o \
	tests/01.o \
	tests/02.o \
	tests/03.o \
	tests/04.o \
	tests/05.o \
	tests/06.o
DEPENDS:= $(OBJECTS:.o=.d)

.PHONY: romdisk example clean

.SUFFIXES: .c .S .o .bin .romdisk

all: example

example: romdisk $(PROJECT).bin

$(PROJECT).bin: $(PROJECT).elf
	$(OB) -O binary $< $@
	@sh -c "du -h -s $@ | cut -d '	' -f 1"

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

.romdisk.o:
	sh ../../tools/bin2o/bin2o $< `echo "$<" | sed -e 's/\./_/g'` $@
.c.o:
	$(CC) $(CFLAGS) -Wp,-MMD,$*.d -c -o $@ $<
.S.o:
	$(AS) $(AFLAGS) -o $@ $<

romdisk:
	../../tools/fsck.genromfs/fsck.genromfs ./romdisk/
	genromfs -a 16 -v -V "ROOT" -d ./romdisk/ -f "root.romdisk"

clean:
	-rm -f $(OBJECTS) ip.o \
$(DEPENDS) \
$(PROJECT).asm \
$(PROJECT).bin \
$(PROJECT).elf \
$(PROJECT).map \
ip.bin \
ip.bin.map \
$(PROJECT).sym

-include $(DEPENDS)
