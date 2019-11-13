#pragma once
#include "core.hpp"

namespace time {

void delay(int cycles = 1) {
    for (int a = 0; a < (1 << 16) * cycles; a++) {
        asm volatile ("and %eax,%eax");
        asm volatile ("and %ecx,%ecx");
    }
}

} // namespace time
