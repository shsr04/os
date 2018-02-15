/* 
	- returning from kernel_main causes a triple fault
*/
#include "kernel.h"
void kernel_main(void);
void print(char*);
void* memcpy(void*,void*,int);

static int x=0;
static shortbuf term=vga_mem;
static int ti=5,tx=5,ty=0;
/* static data is collected in the .data section 
	-> this might be broken: references to x go to ds:0x40! (that is 0x0000000000000040!!)
*/
void kernel_main(void) {
	/* on entry: push ebp, mov esp to ebp, esp:=esp-(size of local vars) */
	/* local variables are collected on the stack as follows:
		mov REG,<value>			/	lea REG,[ebp-<i>]	(to store an address)
		mov [ebp-<i>],REG			mov [ebp-<j>],REG
	*/
	volatile char s[30]="volatile!";
	print((void*)s);
	/*for(x=10; x<20; x++) {
		term[x]=vga_entry(str[x],vga_color(vga_red,vga_lightgrey));
	}
	for(x=20;x<500;x++){
		term[x]=vga_entry('X',vga_color(vga_cyan,vga_black));
	}
	hang();
	while(1);*/
	return;
	/* on exit: esp:=esp+(size of local vars), pop ebp */
}

void print(char *p) {
	int x=0;
	for(x=0; p[x]; x++) {
		/*if(++tx==vga_width) {
			ty+=1;
			tx=0;
		}*/
		term[ti++]=vga_entry(p[x],vga_color(vga_white,vga_black));
		/*term[ti++]=p[x];*/
	}
}

/* required user-supplied functions */
void* memcpy(void *dest,void *src,int count) {
	if(count==0) return dest;
	/*byte *dp=(byte*)dest;
	byte *sp=(byte*)src;*/
	((byte*)dest)[count]=((byte*)src)[count];
	return memcpy(((byte*)dest)+1,((byte*)src)+1,count-1);
}
