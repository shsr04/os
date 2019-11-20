#pragma once
#include "core.hpp"

namespace rand {

/// Ranlim32, as designed by Press et al. (Numerical Recipes, 3rd edition)
class random_gen {
    uint32 v = 2244614371, w1 = 521288629, w2 = 362436069;
    uint32 u;

  public:
    constexpr random_gen(uint32 seed) : u(seed ^ v) {
        next();
        v = u;
        next();
    }

    constexpr uint32 next(uint32 limit = 0) {
        u = u * 2891336453 + 1640531513;
        v ^= v >> 13;
        v ^= v << 17;
        v ^= v >> 5;
        w1 = 33378 * (w1 & 0xffff) + (w1 >> 16);
        w2 = 57225 * (w2 & 0xffff) + (w2 >> 16);
        uint32 x = u ^ (u << 9);
        x ^= x >> 17;
        x ^= x << 6;
        uint32 y = w1 ^ (w1 << 17);
        y ^= y >> 15;
        y ^= y << 5;
        uint32 r = (x + v) ^ (y + w2);
        if (limit == 0)
            return r;
        else
            return r % limit;
    }
};

} // namespace rand
