; PS/2 keyboard macros
;	each key press/release causes a "make"/"break" scan code to be sent from port 0x60
;	there are 3 scan code sets, the de-facto standard is scan code set 2 (1 is legacy, 3 is optional)
bits 32

;kbdMake1 seems to be reliable for scan code set 2, taken from stanislavs.org/helppc/make_codes.html
;kbdMake2 is supposed to be the scan code set 2, but it fails... ?
; note: kbdMake1 contains a few break codes where necessary
kbdMake1 db 1eh,30h,2eh,20h, 12h,21h,22h,23h, 17h,24h,25h,26h, 32h,31h,18h,19h, 10h,13h,1fh,14h, 16h,2fh,11h,2dh, 15h,2ch
	db 0bh,2,3,4,5,6,7,8,9,0ah
	db 39h,1ch,0fh,2ah,0xaa
	db 0
kbdMake2 db 1ch,32h,21h,23h, 24h,2bh,34h,33h, 43h,3bh,42h,4bh, 3ah,31h,44h,4dh, 15h,2dh,1bh,2ch, 3ch,2ah,1dh,22h, 35h,1ah, 0
chars db 'a','b','c','d', 'e','f','g','h', 'i','j','k','l', 'm','n','o','p', 'q','r','s','t', 'u','v','w','x', 'y','z'
	db '0','1','2','3','4','5','6','7','8','9'
	db ' ',0x0A,0x09,0x0F,0x0E ;0x0A=new line, 0x09=tab, 0x0F=shift in ("shift key"), 0x0E=shift out
	db 0
kbdShifted db 0 ; is shift key pressed?
kbdCommandBuffer dd 0 ;index
times 0x200 db 0 ;buffer
db 0xFF
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
cmp al,0x0F ;shift in
je .shiftPressed
cmp al,0x0E ;shift out
je .shiftReleased
cmp byte [kbdShifted],1
je .upperCase
jmp .end
.upperCase sub al,0x20
jmp .end
.shiftPressed:
mov byte [kbdShifted],1
mov al,0
jmp .end
.shiftReleased:
mov byte [kbdShifted],0
mov al,0
.end:
ret
%macro kbdScancodeToAscii 1-2 kbdMake1
mov al,%1
mov esi,%2
call _kbdScancodeToAscii
%endmacro
_kbdStoreChar: ; kbdStoreChar(al: char,edi: buffer)
mov ebx,edi
add edi,[edi]
add edi,4
mov byte [edi],al
inc word [ebx]
ret
_kbdClearBuffer:
mov eax,[edi]
mov dword [edi],0
add edi,4
.head cmp eax,0
je .end
mov byte [edi],0
inc edi
dec eax
jmp .head
.end ret
%macro kbdStoreChar 1-2 kbdCommandBuffer
push eax
push ebx
push edi
mov al,%1
mov edi,%2
call _kbdStoreChar
pop edi
pop ebx
pop eax
%endmacro
%macro kbdClearBuffer 0-1 kbdCommandBuffer
push eax
push edi
mov edi,%1
call _kbdClearBuffer
pop edi
pop eax
%endmacro
