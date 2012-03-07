/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

OUTPUT_FORMAT("elf32-sh")
OUTPUT_ARCH(sh)
ENTRY(_start)

MEMORY {
        lwram : o = 0x00200000, l = 0x10000000
        hwram : o = 0x06004000, l = 0x01ffe000
}

_stack = 0x08000000;

SECTIONS
{
        .init 0x06004000 :
        {
                KEEP (*(.init))
                . = ALIGN(4);
        } > hwram

        .text :
        {
                *(.text)
                . = ALIGN(4);
        } > hwram

        .fini :
        {
                KEEP (*(.fini))
                . = ALIGN(4);
        } > hwram

        .rodata :
        {
                *(.rodata)
                *all.rodata*(*)
                *(.roda)
                *(.rodata.*)
                . = ALIGN(4);
        } > hwram

        .data :
        {
                _data = . ;
                *(.data)
                *(.data.*)
                ___EH_FRAME_BEGIN__ = . ;
                *(.eh_fram*)
                ___EH_FRAME_END__ = . ;
                _edata = . ;
                . = ALIGN(4);
        } > hwram

        .bss :
        {
                _bss = . ;
                *(.bss)
                *(COMMON)
                _ebss = . ;
                __end__ = _ebss;
                _end = _ebss;
                . = ALIGN(0x10);
        } > hwram

        _lwram_lma = . ;

        .lwram 0x00200000 : AT (_lwram_lma)
        {
                _lwram = . ;
                *(.lwram)
                . = ALIGN(4);
        } > lwram

        /DISCARD/ :
        {
                *(.comment)
                *(.rela.*)
        }
}
