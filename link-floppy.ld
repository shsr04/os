/* any format options should be supplied via the Makefile */
SECTIONS {
	/*ENTRY(kernel_main)*/ /* use -e option for this */
	/*. = 0x8000;*/ /* start of data */
	
	.text 0x0 : ALIGN(32) {
		*(.text)
	}
	.rodata : ALIGN(32) {
		*(.rodata)
	}
	.data : ALIGN(32) {
		*(.data)
	}
	.bss :  ALIGN(32) {
		*(COMMON)
		*(.bss)
	}
	.note : ALIGN(128) {
		*(.note.gnu.build-id)
	}
}
