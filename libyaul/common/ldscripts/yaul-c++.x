. = ALIGN (16);
PROVIDE (___CTOR_LIST__ = .);
LONG (((___CTOR_END__ - ___CTOR_LIST__) / 4) - 2)
KEEP (*(SORT (.ctors.*)))
KEEP (*(.ctors))
LONG (0x00000000)
___CTOR_END__ = .;

. = ALIGN (16);
PROVIDE (___DTOR_LIST__ = .);
LONG (((___DTOR_END__ - ___DTOR_LIST__) / 4) - 2)
KEEP (*(SORT (.dtors.*)))
KEEP (*(.dtors))
LONG (0x00000000)
PROVIDE (___DTOR_END__ = .);
