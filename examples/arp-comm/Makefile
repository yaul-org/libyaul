include ../common.mk

PROJECT= arp-comm
OBJECTS+= arp-comm.o

.PHONY: clean

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $(@) $(<)
.s:
	$(AS) $(AFLAGS) -o $(@) $(<)

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) \
		-o $(@) -L. -L../../build/libyaul -lyaul
	$(OB) -O binary $(@) $(PROJECT).bin
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

clean:
	-rm -f $(OBJECTS) \
	$(PROJECT).asm $(PROJECT).bin $(PROJECT).elf $(PROJECT).map $(PROJECT).sym
