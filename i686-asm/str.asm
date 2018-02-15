; string macros and helper functions
bits 32

_strNumToAscii:
.head mov bl,[esi]
cmp bl,0xFF
je .end
add bl,'0'
mov byte [esi],bl
inc esi
jmp .head
.end:
ret
%macro strNumToAscii 1
mov esi,%1
call _strNumToAscii
%endmacro
%macro itoa 1
mov al,%1
add al,'0'
%endmacro
