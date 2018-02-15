; PS/2 keyboard macros
;	each key press/release causes a "make"/"break" scan code to be sent from port 0x60
;	there are 3 scan code sets, the de-facto standard is scan code set 2 (1 is legacy, 3 is optional)
bits 32

;kbdMake1 seems to be reliable for scan code set 2 (why??)
;kbdMake2 is supposed to be the scan code set 2, but it fails... ?
kbdMake1 db 1eh,30h,2eh,20h, 12h,21h,22h,23h, 17h,24h,25h,26h, 32h,31h,18h,19h, 10h,13h,1fh,14h, 16h,2fh,11h,2dh, 15h,2ch, 0
kbdMake2 db 1ch,32h,21h,23h, 24h,2bh,34h,33h, 43h,3bh,42h,4bh, 3ah,31h,44h,4dh, 15h,2dh,1bh,2ch, 3ch,2ah,1dh,22h, 35h,1ah, 0
chars db 'a','b','c','d', 'e','f','g','h', 'i','j','k','l', 'm','n','o','p', 'q','r','s','t', 'u','v','w','x', 'y','z', 0
_kbdScancodeToAscii: ; (set: esi, code: al) : char
mov edx,0
.head:
mov bl,[esi]
cmp bl,0
je .notfound
cmp al,bl
je .found
inc edx
inc esi
jmp .head
.notfound mov al,0
jmp .end
.found:
mov ebx,chars
add ebx,edx
mov byte al,[ebx]
.end:
ret
_kbdScancodeToAscii_Old: 
; TODO handle shift key (+ other special keys)
cmp al,1ch
je .a
cmp al,32h
je .b
cmp al,21h
je .c
cmp al,23h
je .d
cmp al,24h
je .e
cmp al,2bh
je .f
cmp al,34h
je .g
cmp al,33h
je .h
cmp al,43h
je .i
cmp al,3bh
je .j
cmp al,42h
je .k
cmp al,4bh
je .l
cmp al,3ah
je .m
cmp al,31h
je .n
cmp al,44h
je .o
cmp al,4dh
je .p
cmp al,15h
je .q
cmp al,2dh
je .r
cmp al,1bh
je .s
cmp al,2ch
je .t
cmp al,3ch
je .u
cmp al,2ah
je .v
cmp al,1dh
je .w
cmp al,22h
je .x
cmp al,35h
je .y
cmp al,1ah
je .z
jmp .end
;...
.a mov al,'a'
jmp .end
.b mov al,'b'
jmp .end
.c mov al,'c'
jmp .end
.d mov al,'d'
jmp .end
.e mov al,'e'
jmp .end
.f mov al,'f'
jmp .end
.g mov al,'g'
jmp .end
.h mov al,'h'
jmp .end
.i mov al,'i'
jmp .end
.j mov al,'j'
jmp .end
.k mov al,'k'
jmp .end
.l mov al,'l'
jmp .end
.m mov al,'m'
jmp .end
.n mov al,'n'
jmp .end
.o mov al,'o'
jmp .end
.p mov al,'p'
jmp .end
.q mov al,'q'
jmp .end
.r mov al,'r'
jmp .end
.s mov al,'s'
jmp .end
.t mov al,'t'
jmp .end
.u mov al,'u'
jmp .end
.v mov al,'v'
jmp .end
.w mov al,'w'
jmp .end
.x mov al,'x'
jmp .end
.y mov al,'y'
jmp .end
.z mov al,'z'
jmp .end
;...
.end:
ret
%macro kbdScancodeToAscii 1-2 kbdMake1
mov al,%1
mov esi,%2
call _kbdScancodeToAscii
%endmacro
