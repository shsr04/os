/* TODO 
	- GDT [OK?]
	- IDT [??]
	- IRQ
	- PS/2 ISR

	fix strings:
		- non-volatile strings are always uninitialized
		- strings larger than 30 are always uninitialized
	fix io_readb(0x60):
		- constantly dumps bogus data
*/

#include <stddef.h>
#include "kernel.h"
int str_len(char *s) {
	int a=0;
	while(s[a]) a++;
	return a;
}
void print_str(char *s,word clr) {
	static shortbuf t=(shortbuf)0xB8000;
	static int ti=0;
	int a=0;
	for(a=0; a<str_len(s); a++) {
		if(s[a]=='\n') {
			ti+=vga_width-a;
		}
		else {
			if(ti+a>=(vga_height-2)*vga_width) ti=a=0; /*TODO fix wrap-around*/
			t[ti+a]=vga_entry(s[a],clr);
		}
	}
	ti+=a;
}
void print_ch(char c,word clr) {
	volatile char s[3]={c,0};
	print_str(s,clr);
}

struct gdt_entry {
	word limit_l,base_l;
	byte base_m,access,granul,base_h;
} __attribute__((packed));
struct gdt_entry gdt[3];
void gdt_descr(int i,dword base,dword limit,byte access,byte granul) {
	gdt[i].base_l=base&0xFFFF;
	gdt[i].base_m=(base>>16)&0xFF;
	gdt[i].base_h=(base>>24)&0xFF;
	gdt[i].limit_l=limit&0xFFFF;
	gdt[i].access=access; /* type attributes of the segment */
	gdt[i].granul=((limit>>16)&0x0F)|(granul&0xF0); /* 1B or 4KB granularity? */
}
void setup_gdt(void) {
	gdt_descr(0,0,0,0,0); /* null entry */
	gdt_descr(1,0x00000000,0xFFFFFFFF,0x9A,0xCF); /* CS, selector 0x08 */
	gdt_descr(2,0x00000000,0xFFFFFFFF,0x92,0xCF); /* DS, selector 0x10 */
	load_gdt(sizeof(gdt),&gdt);
}

struct idt_entry {
	word offset_l,selector;
	byte zero,type_attr;
	word offset_h;
} __attribute__((packed));
struct idt_entry idt[256];
void idt_gate(int i,dword offset,word selector,int priv,int type) {
	idt[i].offset_l=offset&0xFFFF;
	idt[i].selector=selector; /* CS? */
	idt[i].zero=0;
	idt[i].type_attr=(1<<7)|((priv&0x3)<<5)|(type&0xF); /*0x0E: i386 interrupt gate*/
	idt[i].offset_h=(offset>>16)&0xFFFF;
}
void handle_interrupt();
void setup_idt_old(void) {
	int a=0;
	for(a=0; a<256; a++) idt_gate(a,&handle_interrupt,0x08,0,0x0E);
	load_idt(sizeof(idt),&idt);
	/*test_idt();*/
}
__attribute__((interrupt))
void handle_interrupt() {
	shortbuf term=(shortbuf)0xB8000;
	term[30]=vga_entry('i',vga_white);
}

#define newstring(name,value) volatile char (name)[30]=(value)
#define print(str) print_str((str),vga_white)
#define printc(ch,fg,bg) print_ch((ch),vga_color(fg,bg))
#define printres(x) printc(i2c(x),vga_lightgrey,vga_lightblue)
#define printerr(x) do { e++; printc(i2c(x),vga_white,vga_red); } while(0)
#define printif(x,e) do { if(x==e) printres(x); else printerr(x); } while(0)
void kernel_main(int code) {
	volatile char s1[30]="Init ";
	byte b=0;
	int e=0;
	int ps2_int1=0,ps2_int2=0,ps2_sys=0,ps2_unb=0,ps2_clk1=0,ps2_clk2=0,ps2_tl1=0,ps2_zb;
	newstring(s2,"  Done.\n");
	
	setup_gdt();
	setup_idt();
	pic_ini();
	
	print(s1);
	printc(i2c(code),vga_red,vga_green);
	/* self tests */
	assert(checkb(1,0)==1);
	/* init ps/2 */
	ps2_dis1();
	ps2_dis2();
	ps2_flo();
	b=ps2_rcb();
	ps2_int1=checkb(b,0);
	ps2_int2=checkb(b,1);
	ps2_sys=checkb(b,2);
	ps2_unb=checkb(b,3);
	ps2_clk1=checkb(b,4);
	ps2_clk2=checkb(b,5);
	ps2_tl1=checkb(b,6);
	ps2_zb=checkb(b,7);
	printc(' ',0,0);
	printres(ps2_int1);
	printres(ps2_int2);
	printif(ps2_sys,1);
	printres(ps2_unb);
	printres(ps2_clk1);
	printres(ps2_clk2);
	printres(ps2_tl1);
	printif(ps2_zb,0);
	b=ps2_clk2<<5|ps2_clk1<<4|ps2_sys<<2; /*clear bit 0,1,6*/
	ps2_scb(b);
	b=ps2_tstc();
	/*printif(b,0x55);*/
	ps2_en1();
	ps2_en2();
	
	
	print(s2);
	
	while(1);
}
/*void _start(void) { kernel_main(); }*/
