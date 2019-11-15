.extern kernel_main

.global start

.set MB_MAGIC, 0x1BADB002       // Multiboot v1 magic
.set MB_FLAGS, (0<<0)|(1<<1)    //  0: boot modules on page boundaries, 1: include memory info
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

hlt
hang:
jmp hang
