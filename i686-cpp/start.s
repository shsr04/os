.extern kernel_main

.global start

.set MB_MAGIC, 0x1BADB002
.set MB_FLAGS, (1<<0)|(1<<1)    //  0: module on page boundaries, 1: memory map
.set MB_CHECKSUM, (0-(MB_MAGIC+MB_FLAGS))

.section .multiboot
.align 4
.long MB_MAGIC
.long MB_FLAGS
.long MB_CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 65536
stack_top:

.section .text
start:
mov stack_top, esp

push eax
push ebx
call kernel_main

hang:
jmp hang
