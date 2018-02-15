; The Bootloader
; recommended reading: Ralf Brown's interrupt list, OSDev wiki

cpu 686
org 7c00h 	; set all labels relative to 7c00h
			; great advantage when computing linear address

boot: ;the bootloader is loaded at 0000:7c00h
mov ax,0002h ; video mode (80x25 VGA)
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
;	(the stack should be 16-bit aligned for calls to C)
;
;	Short info on the cdecl calling convention
;		called function:
;		push ebp		; parameters were pushed onto the stack by the caller
;		mov ebp,esp		; bp:=sp, access args with [ebp+8] and upwards
;		; do task		; local variables may be pushed as needed
;		mov esp,ebp		; sp:=bp, cleans local variables
;		pop ebp			; after returning, the caller will clean the stack
;		ret
align 4
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
%macro putc 1-3 00h,0fh
mov eax,0xb8000
add ax,[vgai]
mov cl,%2
shl cl,4
or cl,%3 ; default: black bg, white fg
mov byte [eax+1],cl ; vga_entry = color<<8 | symbol;
mov byte [eax],%1
add word [vgai],2
%endmacro

loader:
bios_printc '!'
;xchg bx,bx

; retrieve information

; clear interrupts
cli
jmp setup_gdt

; load gdt
;	The GDT: a memory array of 8-byte descriptors
;	Descriptor:
;		limit_l		lower word of the segment limit
;		base_l			"	"	of the segment base
;		base_m		middle byte 	"	"
;		access		bits: 1PPS_TEW0
;						Privilege: descriptor privilige level (ring 0-3)
;						SegmentDescriptorType: 0 for system, 1 for code/data
;						Type: 0 for data segment, 1 for code segment
;						(the following bits have different meanings for a data or code segment:)
;						- ExpandDown or Conforming: 0 for DS(!), 1 for CS(?)
;						- Writable or Readable: 1
;						Accessed: set by the CPU on access, leave at 0
;		limit_h		higher nibble of the limit
;		flags		bits: GDL0 
;						Granularity: interpret limit in 1B/4KB
;						DefaultSize/UpperBound: 
;						 - for CS/SS: 16/32 bit default operation/stack pointer size
;							-> if CS: setting to 1 will cause a mess with opcodes, but enable jmps to 32-bit C code
;						 - for expand-down DS: limit interpreted as 64KB/4GB
;						LongSeg: set if 64-bit segment (only in IA32-e mode)
;		base_h		higher byte of the base
;	Selector:
;		index		13 bits: offset into the table
;		flags		bits: TPP (Table: look in GDT/LDT, Privilige: Request Privilege Level 0-3 access)
;					 (-> 0x08=1,0x10=2,0x1b=3,0x23=4,... in GDT with ring 0)
gdt dq 0 ; the initial entry must be zero
; set up CS
dw 0xFFFF ; limit_l
dw 0x0000 ; base_l
db 0x00 ; base_m
db 1001_1110b ;access (0x9A/0x9E)
db (1100b<<4)|0xF ;flags|limit_h
db 0x00 ;base_h
; set up DS
dw 0xFFFF 
dw 0x0000
db 0x00
db 1001_0010b ;access (0x92)
db (1100b<<4)|0xF
db 0x00
.fin db 0fh
gdtr dw 0000h ; limit (size-1)
dd 0000000h ; base address
;;; Set up the GDT ;;;
setup_gdt:
mov word [gdtr],(gdt.fin-gdt-1)
mov ax,ds
shl eax,4 ; seg*16+offs = linear address
add eax,gdt
;xchg bx,bx
mov dword [gdtr+2],eax 	; only the lower EAX register (aka AX) can be accessed in real mode
lgdt [gdtr] ; load the limit+base into the cpu's GDTR register
;jmp enable_pm

;enable A20
;mov ax,2401h
;int 15h

; enter protected mode
;	In Protected Mode, 32-bit instructions will now work properly
;	mov eax,[long_value] -> eax=10203040
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
jmp 0x08:setup_idt 	; 0x08 is the code segment selector

bits 32 ; the CS has a 32-bit DefaultSize, so this is necessary!

; load idt
;	The IDT: a table of 256 8-byte gate descriptors
;	Gate Descriptor: specifies interrupt/task/trap gate
;	Interrupt Gate:
;		offset_l		lower word of the offset to the interrupt handler
;		selector		(word) segment selector to which to apply the offset
;		zero			reserved zero byte
;		type			bits: 1PP0_S110
;							Privilege: descriptor privilege level (ring 0-3)
;							Size: 16-bit/32-bit gate size
;		offset_h		higher word of the offset
;	the gate for INT x is accessed by [idt+8*x]
%define idt_selector 0x08
%define idt_zero 0x00
%define idt_type 1000_1110b
%macro make_int 2
mov eax,%2
mov word [idt+%1*8+0],ax
mov word [idt+%1*8+2],idt_selector
mov byte [idt+%1*8+4],idt_zero
mov byte [idt+%1*8+5],idt_type
shr eax,16
mov word [idt+%1*8+6],ax
%endmacro

; Interrupt Procedures
;	when the cpu calls an interrupt/exception handling procedure:
;	- pushed on stack (in this order): EFLAGS, CS, EIP, error code (if exception)
int30handler:
mov eax,0xb8000+4
mov dword [eax],'I ' ; test
iret
nul_handler:
iret
clk_handler:
;xchg bx,bx
iret
kbd_handler:
;	if needed, initialize 8042 PS/2 controller first
; incoming byte shows the pressed/released key
pusha
mov dx,71h
jmp 0x08:kernel_area+2

; The IDT is nested in the loader sectors (to prevent overwriting by the kernel sectors)
idt dq 0 ; int 00h
dq 0 ; int 01h
times 256*8-($-idt) db 0 	; fill to 256 entries
.fin db 0fh
idtr dw 0000h ;limit (size-1)
dd 00000000h ;base
;;; Set up the IDT ;;;
setup_idt:
;xchg bx,bx
make_int 08,nul_handler
make_int 14,nul_handler
make_int 30,int30handler
make_int 70h,clk_handler
make_int 71h,kbd_handler
mov word [idtr],(idt.fin-idt-1)
mov dword [idtr+2],idt
lidt [idtr]

; enable interrupts
sti
;xchg bx,bx
int 30 ; test

; enable A20 ; -> moved before enable_pm
;...

; jmp to kernel
goto_kernel:
;xchg bx,bx
call 0x08:kernel_area 	; jump to the pasted kernel (the floppy track 0 is loaded in memory)
				;	7c00h			entry (sector 0)
				;	+200h			beginning of this loader (sector 1)
				;	+((n-1)*200h)	beginning of kernel (sector n)
				;	+x				=> the entry method (best at 0x0)
jmp $
times 512*6-($-$$) db 0
kernel_area: ; 8800h
