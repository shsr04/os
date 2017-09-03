
# using clang enables simple and fast cross-compilation
c = clang 
asm = nasm
target-flag = --target=i686-elf
compile-flags = -std=c89 -Weverything -pedantic -ffreestanding -nostdlib -nostdinc #-fno-builtin
link-flags = -ffreestanding -nostdlib -nostdinc #-fno-builtin
full-bin-flags = -w -std=c89 -ffreestanding -nostdlib
ld-flags = -nostdlib
kernel = kernel.out

.PHONY: sim-bochs sim-qemu clean

default: multiboot.iso
sim: sim-bochs

sim-qemu-bin: $(kernel)
	qemu-system-i386 -kernel $(kernel)

sim-bochs: disk.img
	bochs -f bochs.conf

kernel-files:=kernel.o system.o
	# always put kernel.o first (the bootloader relies on the entry point being at 0x0)

kernel.bin: $(kernel-files)
	# -mX: gcc option that produces 8/16/32-bit code
	@echo --- Linking $(kernel-files) to $@ ---
	ld -melf_i386 -T link-floppy.ld -o $@ $(kernel-files) --oformat binary
	hexdump $@ -C > result-kernel.tmp
	
kernel-artifact = kernel.bin	
disk.img: boot.bin $(kernel-artifact)
	dd if=boot.bin of=$@ bs=512
	dd if=$(kernel-artifact) of=$@ bs=512 seek=2 conv=notrunc #seek = skip n sectors before write
	dd if=/dev/zero of=$@ bs=512 seek=2879 count=1
	hexdump disk.img -C > disk.hex
	

# MOST OF THESE RULES MUST BE EXECUTED FROM THE i686-elf DIRECTORY Makefile
# Thanks!
kernel.elf: kernel.o link-floppy.ld
	@echo --- Linking $^ to $@ ---
	clang $(target-flag) $(link-flags) -T link-floppy.ld -o $@ kernel.o

%.bin: %.asm
	@echo --- Assembling $@ ---
	nasm -fbin -o $@ $<

%.o: %.asm
	@echo --- Assembling $@ ---
	nasm -felf32 -o $@ $<

%.o: %.c
	@echo --- Compiling $@ ---
	clang $(target-flag) -c $(compile-flags) -o $@ $<

git-add:
	git add *.c *.h *.asm *.ld bochs.conf Makefile
	cd i686-elf && git add *.c *.h *.asm *.ld Makefile
	
	
clean:
	find . -type f -iname '*.o' -delete;
	find . -type f -iname '*.bin' -delete;
	find . -type f -iname '*.out' -delete;
	find . -type f -iname '*.img' -delete;
	find . -type f -iname '*.tmp' -delete;
	find . -type f -iname '*.iso' -delete;
	if [ -e ./iso ]; then rm -r iso; fi;
