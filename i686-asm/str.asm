; string macros and helper functions
bits 32

_strNumToChars: ; strNumToChars(esi: string) : string
mov edi,.str
.head mov bl,[esi]
cmp bl,0xFF
je .end
add bl,'0'
mov byte [edi],bl
inc esi
inc edi
jmp .head
.str times 0x100 db 0
.end:
mov eax,.str
ret
%macro strNumToChars 1
push ebx
push esi
push edi
mov esi,%1
call _strNumToChars
pop edi
pop esi
pop ebx
%endmacro
%macro strNumToCharc 1
mov al,%1
add al,'0'
%endmacro
_strCompare:
;xchg bx,bx
mov eax,0
.head mov bl,[esi]
mov cl,[edi]
cmp bl,cl
jne .notequal
cmp bl,0
je .equal
inc esi
inc edi
jmp .head
.notequal mov eax,0
jmp .end
.equal mov eax,1
.end ret
%macro strCompare 2
push ebx
push ecx
push esi
push edi
push %2
push %1
pop esi ;set params safely
pop edi
call _strCompare
pop edi
pop esi
pop ecx
pop ebx
%endmacro
