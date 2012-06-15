include ../common.mk

PROJECT= romdisk
OBJECTS+= romdisk.o root.o
ROMDISK= root.romdisk

.PHONY: clean

.SUFFIXES: .c .s .o .romdisk

.c.o:
	$(CC) $(CFLAGS) -c -o $(@) $(<)
.s:
	$(AS) $(AFLAGS) -o $(@) $(<)

.romdisk.o:
	../../tools/bin2o/bin2o $< 'root_romdisk' $@

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) \
		-o $(@) -L. -L../../build/libyaul -lyaul
	$(OB) -O binary $(@) $(PROJECT).bin
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

clean:
	-rm -f $(OBJECTS) \
	$(PROJECT).asm $(PROJECT).bin $(PROJECT).elf $(PROJECT).map $(PROJECT).sym
