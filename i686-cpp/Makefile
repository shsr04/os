default: kernel.elf

TargetFlags := --target=i686-unknown-none	# magic!
WarningFlags := -Wall -Wextra -Wconversion
CompileFlags := -std=c++17 -Oz -fno-exceptions -fno-rtti -I. -Iresources $(TargetFlags) $(WarningFlags)
LinkFlags := 
Modules := mod_print.elf

# Needs: 
#	LLVM toolchain
# 	mtools xorriso grub-pc-bin (for grub-mkrescue)

#.SILENT:
.PHONY: sim-qemu

kernel.iso: kernel.elf $(Modules) grub.cfg
	mkdir -p isobuild/boot/grub isobuild/modules 
	cp kernel.elf isobuild/boot/
	cp grub.cfg isobuild/boot/grub/
	cp $(Modules) isobuild/modules/
	grub-mkrescue -o $@ isobuild

sim-qemu: kernel.elf
	qemu-system-i386 -kernel $<

kernel.elf: start.o kernel.o
	ld.lld -T link.ld -o $@ $^

%.elf: %.load.o %.o 
	ld.lld -T mod_link.ld -o $@ $^

%.o: %.cpp
	clang++ -c $(CompileFlags) -o $@ $<
%.o: %.s
	clang++ -c -masm=intel $(TargetFlags) -o $@ $<
