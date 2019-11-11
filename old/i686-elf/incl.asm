%macro ef 0
push ebp
mov ebp,esp
%endmacro
%macro lf 0
mov esp,ebp
pop ebp
%endmacro
%ifndef defined_dev_asm
%include "dev.asm"
%endif
%ifndef defined_sys_asm
%include "sys.asm"
%endif
