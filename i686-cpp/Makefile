default: kernel.elf

TargetFlags := --target=i386-unknown-none	# magic!
WarningFlags := -Wall -Wextra -Wconversion
CompileFlags := -std=c++17 -Oz -fno-exceptions -fno-rtti -I. -Iresources $(TargetFlags) $(WarningFlags)
LinkFlags := -T link.ld

# Needs LLVM toolchain

.SILENT:
.PHONY: sim-qemu

sim-qemu: kernel.elf
	qemu-system-i386 -kernel $<

kernel.elf: start.o kernel.o
	ld.lld $(LinkFlags) -o $@ $^

%.o: %.cpp
	clang++ -c $(CompileFlags) -o $@ $<
%.o: %.s
	clang++ -c $(TargetFlags) -o $@ $<
