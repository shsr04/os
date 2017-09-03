#include "kernel.h"
static int x=0;
extern hang();
void kernel_main(void) {
	shortbuf term=vga_mem;
	for(x=20;x<500;x++){
		term[x]=vga_entry('X',vga_color(vga_cyan,vga_black));
	}
	hang();
	while(1); /* this causes a triple fault in Bochs (does not like endless loops?) */
}
