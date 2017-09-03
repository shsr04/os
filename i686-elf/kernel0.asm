cpu 686
; word size:
;	i386	2 Bytes


multiboot_all4kaligned equ 1
multiboot_withmemorymap equ 0
flags equ multiboot_all4kaligned | multiboot_withmemorymap<<1
magic equ 0x1BADB002
checksum equ -(magic+flags)

section .multiboot
align 4
dd magic
dd flags
dd checksum

section .bss
align 4
stack_bottom:
resb 16384
stack_top:

section .text
global kernel_boot:function
kernel_boot:
mov esp,stack_top

; - load GDT [OK]
; - enable paging

extern kernel_main
mov eax,42
push eax
call kernel_main

.hang: cli
jmp .hang
