OUTPUT_FORMAT(binary)
SECTIONS
{
	ENTRY(kernel_main)
	/*. = 1M;*/ /* start of data */
	
	.text 0x0 : ALIGN(4K) {
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
}
