; system file - to be called from kernel.c

cpu 686
bits 32

global hang
hang jmp hang

[section .data]
global str
str:
	db 'Hello.',0
	db 0

