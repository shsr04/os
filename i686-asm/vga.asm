; vga macros for the kernel
bits 32
%define vgaBlack 0
%define vgaBlue 1
%define vgaGreen 2
%define vgaCyan 3
%define vgaRed 4
%define vgaMagenta 5
%define vgaBrown 6
%define vgaLightGrey 7
%define vgaDarkGrey 8
%define vgaLightBlue 9
%define vgaLightGreen 10
%define vgaLightCyan 11
%define vgaLightRed 12
%define vgaLightMagenta 13
%define vgaLightBrown 14
%define vgaWhite 15
section .data
align 4 ; align all data which will be accessed by eax,... as dwords!
vgai dw 8 ; the VGA index (one char is 2 bytes wide, => vgai=2*vgax)
vgax db 8 ; x coordinate of the cursor
vgay db 0 ; y coordinate
VGAW equ 80 ; screen width
VGAH equ 25 ; screen height
VGAL equ 80*25-1
vgaEsc db 0
%macro vgaColor 2 ;vgaColor(bg,fg)
mov cl,%1
shl cl,4
or cl,%2
%endmacro
_printc: ;_printc(bl: char, cl: color)
mov eax,0xb8000
add ax,[vgai]
cmp bl,'\'
je .esc1
cmp byte [vgaEsc],1
je .esc2
mov byte [eax+1],cl ; vga_entry = color<<8 | symbol;
mov byte [eax],bl
add word [vgai],2
add byte [vgax],1
cmp byte [vgax],80
je .wrap
jmp .end
.wrap
mov byte [vgax],0
add byte [vgay],1
jmp .end
.esc1:
mov byte [vgaEsc],1
jmp .end
.esc2:
mov byte [vgaEsc],0
cmp bl,'n'
je .esc2n
jmp .end
.esc2n ; newline ('\n')
cmp byte [vgax],VGAW
je .wrap
add byte [vgax],1
add word [vgai],2
jmp .esc2n
.end:
ret
%macro printc 1-3 00h,0fh
push eax
push ebx
push ecx
mov bl,%1
vgaColor %2,%3 ; default: black bg, white fg
call _printc
pop ecx
pop ebx
pop eax
%endmacro
%macro printsi 1-3 00h,0fh ; TODO fix
prints %%str,%2,%3
jmp %%end
%%str: db %1
%%end:
%endmacro
%macro prints 1-3 00h,0fh
mov esi,%1
%%head: mov byte bl,[esi]
cmp bl,00h
je %%end
printc bl,%2,%3
inc esi
jmp %%head
%%end:
%endmacro
%macro vgaClear 0-1 00h ; TODO fix (problem??)
push ebx
mov ebx,0
%%head:
cmp ebx,VGAL
jge %%end
printc ' '
inc ebx
jmp %%head
%%end:
mov dword [vgai],0
pop ebx
%endmacro
