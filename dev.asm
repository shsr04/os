
; Calling convention (cdecl)
;	-> incoming arguments: [ebp+8],...
;	<- outgoing value: eax

section .text

global io_sendb
io_sendb:
push ebp
mov ebp,esp
mov word ax,[ebp+8]
mov [dat.word],ax
mov byte al,[ebp+10]
mov dx,[dat.word]
out dx,al
mov esp,ebp
pop ebp
ret

global io_readb
io_readb:
push ebp
mov ebp,esp
mov word ax,[ebp+8]
mov dx,ax
in al,dx
mov esp,ebp
pop ebp
ret

; sample function
global add
add: ;int,int => int
push ebp
mov ebp,esp
mov eax,[ebp+8]
mov ebx,[ebp+12]
add eax,ebx
leave
ret ;-> returns eax

section .data
dat:
.word 
times 10 dw 0000h
.byte
times 10 db 00h
