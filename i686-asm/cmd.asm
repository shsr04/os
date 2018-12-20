; the command line
; uses:
;	strCompare		str.asm
;	print*			vga.asm
;	clkTicks		clk.asm
cpu 686
bits 32

cmdList dd _cmdTime.name,0
cmdAdrList dd _cmdTime,0

_cmdTime:
strNumToChars clkTicks
prints eax ;TODO fix weird output
printsi {'\n',0}
mov eax,1
ret
.name db 'time',0
_cmdExec: ;cmdExec(esi: command) : boolean
mov ecx,0
.head mov ebx,cmdList
add ebx,ecx
cmp dword [ebx],0
je .notfound
strCompare dword [ebx],esi
;xchg bx,bx
cmp eax,1
je .found
add ecx,4
jmp .head
.found mov ebx,cmdAdrList
add ebx,ecx
call [ebx] ; returns the command's success code
jmp .end
.notfound mov eax,0
.end ret
%macro cmdExec 1
push ebx
push ecx
push esi
mov esi,%1
call _cmdExec
pop esi
pop ecx
pop ebx
%endmacro
