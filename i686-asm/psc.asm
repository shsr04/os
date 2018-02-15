; PS/2 controller macros
; see 8042 PS/2 Controller datasheet
;	port 0x60	data in/out
;	port 0x64	status out/command in
bits 32

; status register (read of 0x64)
; bits: PRTK CSIO
;	P	odd/even parity
;	R	receive timeout?
;	T	transmit timeout?
;	K	keyboard enabled?
;	C	input register holds data/command
;	S	system is running?
;	I	input register holds data? (system->controller)
;	O	output register holds data? (controller->system)
%macro pscCanRead 0 ; pscCanRead() : boolean
in al,0x64
and al,01b
%endmacro
%macro pscCanWrite 0 ; pscCanWrite() : boolean
in al,0x64
and al,10b
cmp al,0
je %%yes
%%no mov al,0
jmp %%end
%%yes mov al,1
%%end
%endmacro
%macro pscRead 0 ; pscRead() : byte
%%head pscCanRead
cmp al,1
jne %%head
in al,0x60
%endmacro
%macro pscWrite 1 ; pscWrite(byte)
%%head pscCanWrite
cmp al,1
jne %%head
mov al,%1
out 0x60,al
%endmacro
%macro pscSelfTest 0
mov al,0xaa
out 0x64,al
pscRead
%endmacro
