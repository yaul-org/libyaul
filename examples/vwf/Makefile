include ../common.mk

PROJECT:= vwf
OBJECTS+= root.o vwf.o
DEPENDS:= $(OBJECTS:.o=.d)

.PHONY: ip clean

.SUFFIXES: .c .S .o .bin .romdisk

example: $(PROJECT).bin

$(PROJECT).bin: $(PROJECT).elf
	$(OB) -O binary $< $@

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

clean:
	-rm -f $(OBJECTS) ip.o \
$(DEPENDS) \
$(PROJECT).asm \
$(PROJECT).bin \
$(PROJECT).elf \
$(PROJECT).map \
ip.bin.map \
$(PROJECT).sym

-include $(DEPENDS)
