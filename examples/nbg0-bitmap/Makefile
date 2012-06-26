include ../common.mk

PROJECT:= nbg0-bitmap
OBJECTS+= nbg0-bitmap.o
DEPENDS:= $(OBJECTS:.o=.d)

.PHONY: bootstrap clean

.SUFFIXES: .c .S .o .bin

example: $(PROJECT).bin

$(PROJECT).bin: $(PROJECT).elf
	$(OB) -O binary $< $@

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

.c.o:
	$(CC) $(CFLAGS) -Wp,-MMD,$*.d -c -o $@ $<
.S.o:
	$(AS) $(AFLAGS) -o $@ $<

clean:
	-rm -f $(OBJECTS) bootstrap.o \
$(DEPENDS) \
$(PROJECT).asm \
$(PROJECT).bin \
$(PROJECT).elf \
$(PROJECT).map \
$(PROJECT).sym

-include $(DEPENDS)
