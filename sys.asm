
section .data
gdtref	dw 0 ;limit
		dd 0 ;base
		
idtref	dw 0 ;limit
		dd 0 ;base

idt:
times 256 resq 0

section .text
global int_hdl
int_hdl: ; ? => .
pushad
cld
extern handle_interrupt
call handle_interrupt
popad
reti

global gen_int
gen_int: ; int => .
ef
mov byte al,[ebp+8]
mov byte [.gen+1],al
.gen int 0xff ;modify 0xCD 0xFF to 0xCD <al>
lf
ret

global load_gdt
load_gdt:
ef
mov word ax,[ebp+8]
mov [gdtref],eax
mov dword eax,[ebp+10]
mov [gdtref+2],eax
lgdt [gdtref]
jmp 0x08:.reload ; FAILS (jump to nowhere)
hlt
.reload:
mov ax,0x10
mov ds,ax
mov es,ax
lf
ret

 ;;; Interrupt Descriptor Table
global setup_idt
extern handle_interrupt
setup_idt:
ef
mov cx,0
.enter
imul bx,cx,8
add bx,idt
mov dx,handle_interrupt
and dx,0xffff
mov word [bx],dx	; offset [15..0]
mov dx,0x08
mov word [bx+2],dx	; selector
mov dx,0xff00
and dh,0b10001110
mov word [bx+4],dx	; type_attr (15: present, 14-13: DPL, 11-9: interrupt gate, 7-0: set to zero)
mov dx,handle_interrupt
shr dx,16
and dx,0xffff
mov word [bx+6],dx	; offset [31..16]
inc cx
cmp cx,255
jne .enter
mov word [idtref],256*8
mov dword [idtref+2],idt
lidt [idtref]
lf
ret

global load_idt
load_idt:
ef
mov word ax,[ebp+8]
mov [idtref],ax
mov dword eax,[ebp+10]
mov [idtref+2],eax
lidt [idtref]
lf
ret

global test_idt
test_idt: 	; FAIL
ef
mov eax,int_hdl
mov word [idt+49*8],ax
mov word [idt+49*8+2],0x08
mov word [idt+49*8+4],0x8E00
shr eax,16
mov word [idt+49*8+6],ax
int 49
lf
ret

; -- Stack map --
;	ebp+12	...
;	ebp+8	first param
;	ebp+4	old eip
;	ebp+0	old ebp
;	ebp-4	first local var
;	ebp-8	...
