#pragma once
#include "core.hpp"

namespace rand {

/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.
*/

/* Period parameters */

class random_gen {
    static constexpr uint32 N = 624;
    static constexpr uint32 M = 397;
    static constexpr uint32 MATRIX_A = 0x9908b0dfUL; /* constant vector a */
    static constexpr uint32 UPPER_MASK =
        0x80000000UL; /* most significant w-r bits */
    static constexpr uint32 LOWER_MASK =
        0x7fffffffUL; /* least significant r bits */

    static constexpr array<uint32, 2> MAG01{0x0UL, MATRIX_A};

    array<uint32, N> mt; /* the array for the state vector  */
    uint32 mti = N + 1;     /* mti==N+1 means mt[N] is not initialized */

  public:
    constexpr random_gen(uint32 seed = 5489) {
        mt[0] = seed & static_cast<uint32>(~0);
        for (mti = 1; mti < N; mti++) {
            mt[mti] =
                (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
            mt[mti] &= 0xffffffffUL;
        }
    }

    constexpr uint32 next() {
        uint32 y = 0;

        if (mti >= N) { /* generate N words at one time */
            uint32 kk = 0;

            for (; kk < N - M; kk++) {
                y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
                mt[kk] = mt[kk + M] ^ (y >> 1) ^ MAG01[y & 0x1UL];
            }
            for (; kk < N - 1; kk++) {
                y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
                mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ MAG01[y & 0x1UL];
            }
            y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
            mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ MAG01[y & 0x1UL];

            mti = 0;
        }

        y = mt[mti++];

        /* Tempering */
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);

        return y;
    }
};

} // namespace rand
