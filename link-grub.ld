SECTIONS 
{
	ENTRY(kernel_boot)
	/*. = 1M;*/ /* start of data */
	
	/* make multiboot header start at 0x0 */
	.multiboot 0x0 : ALIGN(4K) {
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
	.note.gnu.build.id 0x100 : { *(.note.gnu.build-id) }
}
