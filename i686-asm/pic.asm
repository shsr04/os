; PIC macros - model 8259A (see Intel datasheet)
; The programmable interrupt controller consists of PIC1 and PIC2 with the following I/O ports
; PIC1: control 0x20, data 0x21
; PIC2: control 0xa0, data 0xa1
;	(in the datasheet, A0=0 means 0x20 and A0=1 translates to 0x21 for PIC1)
; in cascade mode, PIC2 acts as a slave and interrupts are cascaded
; at system startup:
;	IRQ 0..7 = INT 08h..0Fh (=> default offset 08h)
;	IRQ 8..15 = 70h..77h (=> default offset 70h)
bits 32
section .data
align 4
pic1 db 0
pic2 db 0
%macro picRemap 0-2 0x70,0x78
cli
;send initialization command word 1, clears current PIC status
; ICW1: 0001 LISC
;	L	edge/level triggered mode
;	I	interval of call addresses (4/8 bytes)
;	S	cascade/single mode
;	C	ICW4 not needed/needed
outb 0x20,0001_0001b
outb 0xa0,0001_0001b
;ICW 2, adjust interrupt offset
; PIC1 will cause interrupts 0x70..0x77, PIC2 will cause 0x78..0x7F (default params)
outb 0x21,%1
outb 0xa1,%2
;ICW 3 (only in cascade mode)
; master: has slave on line 0..7?
; slave: 0000_0III
;	I	slave identity (number of the master line)
;		master has slave on line S2 (master's bit 2 is set), so the slave ID is 0x02 
outb 0x21,0000_0100b
outb 0xa1,0000_0010b
;ICW 4, more adjustments (only if specified in ICW1)
; ICW4: 000S BBAM
;	S	special fully nested mode?
;	B	buffered mode (if yes, slave/master)?
;	A	auto EOI?
;	M	MCS-80 mode/8086 mode
outb 0x21,0000_0011b
outb 0xa1,0000_0011b
;send operation command word 1
; interrupt mask (enable/disable interrupt lines 0..7)
;	standard IRQ numbers:
;	0	clock (PIT)
;	1	keyboard
;	6	floppy
;	8	CMOS real-time clock
;	12	mouse
;	14	ATA disk 1
;	15	ATA disk 2
outb 0x21,1111_1100b ; PIC1 receives clock and keyboard IRQs (see boot.asm interrupt handlers)
outb 0xa1,1111_1111b ; PIC2 is fully masked
sti
%endmacro
%macro picRemask 0-2 0x0,0x0
mov al,%1
out 0x21,al
mov al,%2
out 0xa1,al
%endmacro
%macro picReadISR 0 ;which IRQs are being serviced?
;send "OCW3" command word
outb 0x20,0x0b
outb 0xa0,0x0b
in al,0x20
mov [%%r1],al
in al,0xa0
mov [%%r2],al
mov al,[%%r1]
mov ah,[%%r2] ; eax = pic2|pic1
jmp %%end
%%r1 db 0
%%r2 db 0
%%end:
%endmacro
%macro ioWait 0-1 0
cmp %1,0
je %%port
%%cycle0 jmp %%cycle1
%%cycle1 jmp %%end
%%port: out 0x80,0
%%end:
%endmacro

;in/out only work with the al register
%macro inb 2
in al,%1
mov [%2],al
%endmacro
%macro outb 2
mov al,%2
out %1,al
%endmacro
