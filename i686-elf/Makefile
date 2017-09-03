
# using clang enables simple and fast cross-compilation
c = clang 
asm = nasm
compile-flags = -g --target=i686-elf -std=c89 -w -ffreestanding -nostdlib #-fno-builtin -nostdinc
link-flags = --target=i686-elf -ffreestanding -nostdlib #-fno-builtin -nostdinc
full-bin-flags = -w -std=c89 -ffreestanding -nostdlib
kernel = kernel.out

.PHONY: sim-bochs sim-qemu clean

default: multiboot.iso
sim: sim-qemu-grub

multiboot.iso: $(kernel) iso/boot/grub/grub.cfg
	grub-file --is-x86-multiboot $(kernel);
	cp $(kernel) iso/boot;
	grub-mkrescue -o $@ iso;
	
iso/boot/grub/grub.cfg: $(kernel)
	mkdir -p iso/boot/grub;
	printf "menuentry \"kernel\" { \n\tmultiboot /boot/$(kernel) \n}" > iso/boot/grub/grub.cfg 

sim-qemu-bin: $(kernel)
	qemu-system-i386 -kernel $(kernel)

sim-qemu-grub: multiboot.iso
	qemu-system-i386 -cdrom multiboot.iso

disk-v0.img: disk.asm
	nasm -o $@ $<
	
disk.img: boot.bin kernel.bin
	# ??? dd überschreibt alles?
	dd if=boot.bin of=$@ bs=512 count=4
	dd if=kernel.bin of=$@ bs=512 seek=4 #seek = skip n sectors before write
	dd if=/dev/zero of=$@ bs=512 seek=2879 count=1
	hexdump disk.img -C > disk.hex
	
disk-v2.img: boot.asm kernel.c link-floppy.ld
	#experimental: try to load C kernel
	nasm -felf32 -o boot.o boot.asm
	clang -c -m16 $(compile-flags) -o kernel.o kernel.c
	clang -m16 $(link-flags) -T link-floppy.ld -o kernel.bin boot.o kernel.o
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=kernel.bin of=$@
	
sim-bochs: disk.img
	bochs -f bochs.conf

kernel-files = kernel0.o kernel.o
kernel.out: $(kernel-files) link-grub.ld
	@echo --- Linking $^ to $@ ---
	clang $(link-flags) -T link-grub.ld -o $@ $(kernel-files)

%.bin: %.asm
	nasm -fbin -o $@ $<

%.o: %.asm
	@echo --- Assembling $@ ---
	nasm -felf32 -o $@ $<

kernel.o: kernel.c kernel.h
%.o: %.c
	@echo --- Compiling $@ ---
	clang -c $(compile-flags) -o $@ $<

%.bin: %.c
	#TODO strip kernel.c of extern decls
	clang $(full-bin-flags) -T link-floppy.ld -o $@ $<
	
clean:
	find . -type f -iname '*.o' -delete;
	find . -type f -iname '*.bin' -delete;
	find . -type f -iname '*.out' -delete;
	find . -type f -iname '*.img' -delete;
	find . -type f -iname '*.hex' -delete;
	if [ -e ./iso ]; then rm -r iso; fi;
