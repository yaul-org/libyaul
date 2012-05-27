include ../common.mk

PROJECT= nbg0-bitmap
OBJECTS+= image.o nbg0-bitmap.o

.PHONY: clean

.SUFFIXES: .c .o

.c.o:
	$(CC) $(CFLAGS) -c -o $(@) $(<)
.s:
	$(AS) $(AFLAGS) -o $(@) $(<)

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) \
		-o $(@) -L../../build/libyaul -lyaul
	$(OB) -O binary $(@) $(PROJECT).bin
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

clean:
	-rm -f $(OBJECTS) \
		$(PROJECT).asm $(PROJECT).bin $(PROJECT).elf $(PROJECT).map $(PROJECT).sym
