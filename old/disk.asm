; 1.44MB floppy disk

%include "boot.asm"

times 1440*1024-($-$$) db 00h ;fill entire floppy
