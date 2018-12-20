/*
 Programming Guide
	- no modifications of a variable allowed
		- exception: a reference parameter can be modified, if needed

*/

/* TODO
 - cannot write at terminal_buffer[0]
 - weird(!) string behaviour

*/
#define _pure_functional
#include "kernel.h"

int ret1,ret2,ret3,ret4;

int string_length(char *str,int i) {
	if(str[i]) return string_length(str,i+1);
	else return i;
}
int strlen(char *str) {
	return string_length(str,0);
}

void terminal_clear(shortbuf buffer,unsigned char color,int x,int y) {
	if(x==vga_width) terminal_clear(buffer,color,0,y+1);
	if(y==vga_height) return;
	buffer[y*vga_width+x]=vga_entry(' ',color);
}
void termclear(shortbuf buffer,int *row,int *col,unsigned char color) {
	terminal_clear(buffer,color,0,0);
	*row=0;
	*col=0;
}

void terminal_write(shortbuf term,char *data,int length,int row,int col,unsigned char color,int i) {
	if(i==length) {
		ret1=row;
		ret2=col;
	}
	else {
		if(row==vga_height) terminal_write(term,data,length,0,0,color,i);
		if(col==vga_width) terminal_write(term,data,length,row+1,0,color,i);
		term[row*vga_width+col]=vga_entry(data[i],color);
		terminal_write(term,data,length,row,col+1,color,i+1);
	}
}
void termwrite(shortbuf term,char *data,int *row,int *col,unsigned char color) {
	terminal_write(term,data,strlen(data),*row,*col,color,0);
	*row=ret1;
	*col=ret2;
}

#define print(str) terminal_buffer[2]=vga_entry(i2c((str)[0]),vga_white); termwrite(terminal_buffer,str,&terminal_row,&terminal_column,terminal_color)
#define clear(color) termclear(terminal_buffer,&terminal_row,&terminal_column,color)
void kernel_main(void) {
	int terminal_row=0,terminal_column=0;
	byte terminal_color=vga_color(vga_lightgrey,vga_black);
	shortbuf terminal_buffer=(shortbuf)0xB8000;
	char str[][250]={"Started kernel.","$"};
	char *dat="Testing";
	terminal_buffer[1]=vga_entry(dat[0],vga_white); /*wenn auskommentiert: print funkt. nicht!*/
	/*terminal_buffer[3]=vga_entry('#',vga_red);
	terminal_buffer[4]=vga_entry(i2c(strlen(dat)),vga_white);*/
	print(str[0]);
}
void _start(void) { kernel_main(); } /* entry point for linker (not needed, just to be sure) */
