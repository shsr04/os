/* any format options should be supplied via the Makefile */
SECTIONS {
	/*ENTRY(kernel_main)*/ /* use -e option for this */
	/*. = 0x10000;*/ /* start of file */
	
	.text 0x0 : ALIGN(32) {
		*(.text)
	}
	/* data area starts at mem 200KB */
	.data /*0x200000*/ : ALIGN(32) {
		*(.data)
	}
	.rodata /*0x450000*/ : ALIGN(32) {
		*(.rodata)
	}
	.bss /*0x500000*/ :  ALIGN(32) {
		*(COMMON)
		*(.bss)
	}
	.note : ALIGN(128) {
		*(.note.gnu.build-id)
	}
}
