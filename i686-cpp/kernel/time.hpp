#pragma once
#include "core.hpp"

namespace time {

/**
 * Spin the CPU for a given number of "moments".
 * (One "moment" is 2^16 AND operations.)
 */
void delay(int amount = 1) {
    for (int a = 0; a < (1 << 16) * amount; a++) {
        asm volatile ("and %eax,%eax");
    }
}

} // namespace time
