include ../common.mk

PROJECT= dram-cartridge
OBJECTS+= dram-cartridge.o

.PHONY: clean

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $(@) $(<)
.s:
	$(AS) $(AFLAGS) -o $(@) $(<)

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) \
		-o $(@) -L. -L../../build/libyaul -lyaul
	$(OB) -O binary $(@) $(PROJECT).bin
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

clean:
	-rm -f $(OBJECTS) \
	$(PROJECT).asm $(PROJECT).bin $(PROJECT).elf $(PROJECT).map $(PROJECT).sym
