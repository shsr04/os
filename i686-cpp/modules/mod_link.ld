ENTRY(load)

SECTIONS {
    . = 0;
    .text : ALIGN(4) {
        *.load.o(.text)
        *.o(.text)
    }
    .data : ALIGN(4) {
        *(.data)
    }
    .rodata : ALIGN(4) {
        *(.data)
    }
}
