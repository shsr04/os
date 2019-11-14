default: kernel.elf

TargetFlags := --target=i686-unknown-none	# magic!
WarningFlags := -Wall -Wextra -Wconversion
CompileFlags := -std=c++17 -Oz -fno-exceptions -fno-rtti -I. -Iresources $(TargetFlags) $(WarningFlags)
LinkFlags := -T link.ld

# Needs: 
#	LLVM toolchain
# 	mtools xorriso grub-pc-bin (for grub-mkrescue)

#.SILENT:
.PHONY: sim-qemu

kernel.iso: kernel.elf grub.cfg
	mkdir -p isobuild/boot/grub 
	cp kernel.elf isobuild/boot/
	cp grub.cfg isobuild/boot/grub/
	grub-mkrescue -o $@ isobuild

sim-qemu: kernel.elf
	qemu-system-i386 -kernel $<

kernel.elf: start.o kernel.o
	ld.lld $(LinkFlags) -o $@ $^

%.o: %.cpp
	clang++ -c $(CompileFlags) -o $@ $<
%.o: %.s
	clang++ -c -masm=intel $(TargetFlags) -o $@ $<
