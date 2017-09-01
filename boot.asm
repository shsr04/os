; The Bootloader
; recommended reading: Ralf Brown's interrupt list, OSDev wiki

cpu 686
org 7c00h 	; set all labels relative to 7c00h
			; great advantage when computing linear address

boot: ;the bootloader is loaded at 0000:7c00h
mov ax,0002h ; video mode
int 10h
mov ax,0000h ;flat segments (cs=ds=es=ss)
mov ds,ax	; ds and es segments may be loaded differently, but not cs
mov es,ax
mov si,0 ;important! set up source and dest
mov di,0

[section .bss]
; The stack grows from HIGH to LOW mem!
;	[   ] <- sp points to the next free byte
;	[...]
;	[...] <- bp points to the current stack frame
;	etc.
;
;	Short info on the cdecl calling convention
;		called function:
;		push ebp		; parameters were pushed onto the stack by the caller
;		mov ebp,esp		; bp:=sp, access args with [ebp+8] and upwards
;		; do task		; local variables may be pushed as needed
;		mov esp,ebp		; sp:=bp, cleans local variables
;		pop ebp			; after returning, the caller will clean the stack
;		ret
stack resb 2000h
.end:

[section .text]
mov ss,ax ; the stack will be set up again later (after setting up the GDT)
mov sp,stack.end ; stack pointer

mov ax,0211h ;ah=02h, al=read n sectors
mov cx,0002h ;track 0, sector 2 
mov dx,0000h ;head 0, floppy 0
mov bx,7e00h ;es:bx -> base of output buffer
int 13h ; do it!
mov ah,0eh
mov al,'L'
int 10h

;xchg bx,bx ; bochs magic breakpoint
jmp 0:7e00h	; => jump beyond boot sector
			; needs to be a far jump to 0:7e00h (linear address)
			; or (different setup:) load ds with 07c0h, read sectors to 200h and jump to (ds:)200h

times 510-($-$$) db 00h ;$='here', $$=beginning of section
dw 0xAA55

;use only in real mode, or a triple fault (hard reset) will occur
%macro bios_printc 1 
mov byte al,%1
mov ah,0eh
int 10h
%endmacro
%macro bios_print 1
mov si,%1
.loop lodsb
cmp al,0
je .done
printc al
jmp .loop
.done:
%endmacro
%macro ef 0
push ebp
mov ebp,esp
%endmacro
%macro lf 0
mov esp,ebp
pop ebp
%endmacro

; use these VGA methods only in protected mode
vgai dw 4
%macro putc 1
mov eax,0xb8000
add ax,[vgai]
mov bx,%1
mov [eax],0fh ; white fg, black bg
mov [eax+1],bx
mov ax,[vgai]
mov word [vgai],ax+2
%endmacro

loader:
bios_printc '!'
;xchg bx,bx

; clear interrupts
cli
jmp setup_gdt

; load gdt
;	The GDT: a memory array of 8-byte descriptors
;	Descriptor:
;		limit_l		lower word of the segment limit
;		base_l			"	"	of the segment base
;		base_m		middle byte 	"	"
;		access		bits: 1PPSTEW0
;						Privilege: descriptor privilige level (ring 0-3)
;						SegmentClass: 0 for system, 1 for code/data
;						Type: 0 for data segment, 1 for code segment
;						(the following bits have different meanings for a data or code segment:)
;						- ExpandDown or Conforming: 0?
;						- Writable or Readable: 1
;						Accessed: set by the CPU on access, leave at 0
;		limit_h		higher nibble of the limit
;		flags		bits: GDL0 
;						Granularity: interpret limit in 1B/4KB
;						DefaultOpSize: 16/32 bit default operation size
;						LongSeg: set if 64-bit segment (only in IA32-e mode)
;		base_h		higher byte of the base
;	Selector:
;		index		13 bits: offset into the table
;		flags		bits: TPP (Table: look in GDT/LDT, Privilige: Request Privilege Level 0-3 access)
;					 (-> 0x08=1,0x10=2,0x1b=3,0x23=4,... in GDT with ring 0)
%define limit_l(limit_qw) ((limit_qw)&0xFFFF)
%define base_l(base_qw) ((base_qw)&0xFFFF)
%define base_m(base_qw) (((base_qw)>>16)&0xFF)
%define access(isCodeSeg) ((1<<7)|(1<<4)|((isCodeSeg)<<3)|(1<<1))
%define limit_h(limit_qw) (((limit_qw)>>16)&0x0F)
%define flags(granul) (((granul)<<4)|(1<<3))
%define base_h(base_qw) (((base_qw)>>24)&0xFF)
%define segbase 0x0
%define seglimit 0xffffFFFF
gdt dq 0 ; the initial entry must be zero
dw limit_l(seglimit) ; set up CS
dw base_l(segbase)
db base_m(segbase)
db access(1)
db (flags(0)<<4)|limit_h(seglimit)
db base_h(segbase)
dw limit_l(seglimit) ; set up DS
dw base_l(segbase)
db base_m(segbase)
db access(0)
db (flags(0)<<4)|limit_h(seglimit)
db base_h(segbase)
.fin db 0fh
gdtr dw 0000h ; limit (size)
dd 0000000h ; base address
;;; Set up the GDT ;;;
setup_gdt:
mov word [gdtr],(gdt.fin-gdt)
mov ax,ds
shl eax,4 ; seg*16+offs = linear address
add eax,gdt
mov dword [gdtr+2],eax 	; only the lower EAX register (aka AX) can be accessed in real mode
lgdt [gdtr] ; load the limit+base into the cpu's GDTR register
jmp enable_pm

; enter protected mode
;	In Protected Mode, 32-bit instructions will now work properly
;	mov eax,[gdtr] -> eax=7e0d0018
enable_pm:
mov eax,cr0
or eax,1
mov cr0,eax

; load segment selectors
mov ax,0x10
mov ds,ax
mov es,ax
mov fs,ax
mov gs,ax
;xchg bx,bx
mov ss,ax
mov sp,stack.end
jmp 0x08:load_idt ; 0x08 is the code segment selector

; load idt
;	The IDT: a table of 256 8-byte gate descriptors
;	Gate Descriptor: specifies interrupt/task/trap gate
;	Interrupt Gate:
;		offset_l		lower word of the offset to the interrupt handler
;		selector		(word) segment selector to which to apply the offset
;		zero			reserved zero byte
;		type			bits: 1PP0S110
;							Privilege: descriptor privilege level (ring 0-3)
;							Size: 16-bit/32-bit gate size
;		offset_h		higher word of the offset
;	the gate for INT x is accessed by [idt+8*x]
;%define offset_l(offset_dw) ((offset_dw)&0xFFFF)
%macro offset_l 2
mov eax,%2
and eax,0xFFFF
mov word [idt+%1*8+0],ax
%endmacro
%define selector 0x08
%define zero 0x00
%define type(size) ((1<<7)|((size)<<3)|(1<<2)|(1<<1))
;%define offset_h(offset_dw) (((offset_dw)>>16)&0xFFFF)
%macro offset_h 2
mov eax,%2
shr eax,16
and eax,0xFFFF
mov word [idt+%1*8+24],ax
%endmacro
; Interrupt Procedures
;	when the cpu calls an interrupt/exception handling procedure:
;	- pushed on stack (in this order): EFLAGS, CS, EIP, error code (if exception)
int30handler:
mov eax,0xb8000+4
mov dword [eax],'I ' ; test
pop eax ; XXX begin of dirty fix (eip and cs are pushed as dwords by system)
pop ebx
push bx
push ax ; XXX end of dirty fix
iret
[section .data] ; IDT must be in .data section to avoid faulty jmps
idt dq 0 ; int 00h
dq 0 ; int 01h
times 28 dq 0
.int30:
	dw 0
	dw selector
	db zero
	db type(1)
	dw 0
times 256*8-($-idt) db 0 	; fill to 256 entries
.fin db 0fh
idtr dw 0000h ;limit
dd 00000000h ;base
[section .text]
load_idt:
offset_l 30,int30handler
offset_h 30,int30handler
mov word [idtr],idt.fin-idt
mov dword [idtr+2],idt
lidt [idtr]

; enable interrupts
sti
;xchg bx,bx
int 30 ; test
;jmp $

; enable A20 (Fast A20)
in al,0x92
or al,2
out 0x92,al

; interpret kernel ELF data(?)
; jmp to kernel
kernel_main dd 0 ; this will hold the address of the main method
xchg bx,bx
jmp 0x08:8400h 	; jump to the pasted kernel (the floppy track 0 is loaded in memory)
				;	7c00h			entry (sector 0)
				;	+200h			this loader (sector 1..i)
				;	+((n+1)*200h)	beginning of kernel (sector n)
				;	+x				=> the extracted entry method
hlt
