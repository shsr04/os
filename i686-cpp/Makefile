default: kernel.elf

WarningFlags := -Wall -Wextra -Wconversion
CompileFlags := -std=c++17 $(WarningFlags) -Oz -fno-exceptions -fno-rtti --target=i386-unknown-none
LinkFlags := -T link.ld

# Needs LLVM toolchain

.PHONY: sim-qemu

sim-qemu: kernel.elf
	qemu-system-i386 -kernel $<

kernel.elf: start.o kernel.o
	ld.lld $(LinkFlags) -o $@ $^

%.o: %.cpp
	clang++ -c $(CompileFlags) -o $@ $<
%.o: %.s
	clang++ -c $(CompileFlags) -o $@ $<
