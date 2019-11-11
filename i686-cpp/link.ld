ENTRY(start)

SECTIONS 
{
	. = 1M; /* start of data, below 1M is reserved for x86 stuff */
	
	/* multiboot header */
	.rodata : ALIGN(4K) {
		*(.multiboot)
	}

	.text : ALIGN(4K) {
		*(.text)
	}
	
	.rodata : ALIGN(4K) {
		*(.rodata)
	}
	
	.data : ALIGN(4K) {
		*(.data)
	}

	.bss : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}
	//.note.gnu.build.id 0x100 : { *(.note.gnu.build-id) }
}
