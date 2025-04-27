ENTRY(start)

SECTIONS
{
    . = 1M;

    .text :
    {
        KEEP(*(.multiboot))
        *(.text)
        *(.text.*)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}
