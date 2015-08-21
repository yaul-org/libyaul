DEPENDS:= ${OBJECTS:.o=.d}

all: example

example: $(PROJECT).bin

IP.BIN: ip.o
	$(CC) -Wl,-Map,$@.map -nostdlib -m2 -mb -nostartfiles \
		-T $(ROOTDIR)/common/ldscripts/ip.x \
		$< -o $@

image: IP.BIN example
	cp $(PROJECT).bin cd/A.BIN
	sh $(ROOTDIR)/../tools/make-iso/make-iso $(PROJECT)

$(PROJECT).bin: $(PROJECT).elf
	$(OB) -O binary $< $@
	@sh -c "du -h -s $@ | cut -d '	' -f 1"

$(PROJECT).elf: $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@
	$(NM) $(PROJECT).elf > $(PROJECT).sym
	$(OD) -S $(PROJECT).elf > $(PROJECT).asm

%.romdisk: $(shell find ./romdisk -type f 2> /dev/null) $(ROMDISK_DEPS)
	genromfs -a 16 -v -V "ROOT" -d ./romdisk/ -f $@

%.romdisk.o: %.romdisk
	$(ROOTDIR)/../tools/fsck.genromfs/fsck.genromfs ./romdisk/
	sh $(ROOTDIR)/../tools/bin2o/bin2o $< `echo "$<" | sed -E 's/[\. ]/_/g'` $@

%.o: %.c
	$(CC) $(CFLAGS) -Wp,-MMD,$*.d -c -o $@ $<

%.o: %.S
	$(AS) $(AFLAGS) -o $@ $<

clean:
	-rm -f $(OBJECTS) ip.o \
		$(DEPENDS) \
		$(PROJECT).asm \
		$(PROJECT).bin \
		$(PROJECT).elf \
		$(PROJECT).map \
		IP.BIN \
		IP.BIN.map \
		$(PROJECT).sym

-include $(DEPENDS)
