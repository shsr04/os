
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

default: disk.img
sim: sim-bochs

sim-qemu-bin: $(kernel)
	qemu-system-i386 -kernel $(kernel)

sim-bochs: disk.img
	bochs -f bochs.conf

kernel-bin := kern.bin
disk.img: boot.bin $(kernel-bin)
	dd if=boot.bin of=$@ bs=512
	dd if=$(kernel-bin) of=$@ bs=512 seek=6 conv=notrunc #seek = skip n sectors before write
	dd if=/dev/zero of=$@ bs=512 seek=2879 count=1
	hexdump disk.img -C > disk.hex
	

# Check the i686-elf directory for the GRUB module
kernel.elf: $(kernel-files) link-floppy.ld
	@echo --- Linking $(kernel-files) to $@ ---
	clang $(target-flag) $(link-flags) -T link-floppy.ld -o $@ $(kernel-files)

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
