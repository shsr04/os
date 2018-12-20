; this is the kernel area - proceed with caution
cpu 686
bits 32
org 8800h ; adjusted to boot loader

jmp kernel_start ; 8800h
jmp isr ; 8802h

section .data
; be careful when accessing word-sized constants with eax etc.
; better: align all constants as dwords (= 4 bytes, as NASM syntax is traditionally 16-bit oriented)
align 4
%include "vga.asm"
%include "pic.asm"
%include "kbd.asm"
%include "psc.asm"
%include "clk.asm"
%include "str.asm"
%include "cmd.asm"
cpuVendor times 4 dd 0
cpuIdMax db 0
cpuSignature dd 0
db 0xFF
currentTicks dd 0
db 0xFF

section .text
kernel_start:
vgaClear

;;; CPU identification
mov eax,00h
cpuid
mov [cpuVendor],ebx
mov [cpuVendor+4],edx
mov [cpuVendor+8],ecx
mov [cpuIdMax],eax
mov eax,01h
mov [cpuSignature],eax
cpuid
printsi {'CPU: ',0}
prints cpuVendor
printsi {'\n',0}

;;; initialize PIC
;xchg bx,bx
picRemap
picReadISR

;;; PS/2 keyboard setup
printsi {'PS/2: ',0}
; controller self test
pscSelfTest
cmp al,55h
je .psc1
printc 8,vgaLightGrey,vgaRed
jmp .psc2
.psc1 printc 8,vgaLightGrey,vgaGreen
.psc2:
; enable keyboard
pscWrite 0xF4
pscRead
; keyboard diagnostic echo
pscWrite 0xEE
xor al,al
pscRead
cmp al,0xEE
je .psc3
 printc 8,vgaLightGrey,vgaRed
.psc3 printc 8,vgaLightGrey,vgaGreen
.psc4:
; keyboard, get scan code set
pscWrite 0xF0
pscRead ;ACK
pscWrite 0x00
pscRead ;ACK
pscRead ;scan code number
strNumToCharc al
printc al
printsi {'\n',0}

;;; Paging (see Intel Manual Vol. 3A, section 4.3 "32-bit Paging")
; ...

printsi {'\nHello.',0},vgaRed,vgaGreen

; TODO: 
; - paging?
ret

isr:
cmp dx,70h
je .clk
cmp dx,71h
je .kbd
.end popa 
iret
.clk:
inc dword [clkTicks]
;inc byte [0xb8050]
jmp .end
.kbd:
in al,0x60
;xchg bx,bx
kbdScancodeToAscii al
cmp al,0
je .end
printc al
cmp al,0x0A
je .kbd1
kbdStoreChar al
jmp .end
.kbd1 mov eax,kbdCommandBuffer
add eax,4
cmdExec eax
cmp eax,0
jne .kbd2
mov eax,kbdCommandBuffer
add eax,4
prints eax
printsi {' not found\n',0}
.kbd2 kbdClearBuffer
jmp .end

