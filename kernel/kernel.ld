OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0x400000000000;
    _KernelStart = .;
    .text ALIGN(0x1000) : AT (ADDR (.text) - 0x400000000000){
        *(.text*)
    }

    .rodata ALIGN(0x1000) : AT (ADDR (.rodata) - 0x400000000000){
        *(.rodata*)
    }

    _KernelROEnd = .;

    .data ALIGN(0x1000) : AT (ADDR (.data) - 0x400000000000){
        *(.data*)
    }

    .bss ALIGN(0x1000) : AT (ADDR (.bss) - 0x400000000000){
        *(COMMON*)
        *(.bss*)
    }
    _KernelEnd = .;
}
