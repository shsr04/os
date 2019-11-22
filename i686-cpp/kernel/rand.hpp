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

constexpr uint32 hash(int x) {
    uint32 r =
        1372383749 * static_cast<uint32>(x) + 1289706101; ///< LCG base 32
    r ^= r >> 3;                                          ///< xor-shift R
    r ^= r << 13;
    r ^= r >> 7;
    r *= 1914874293; ///< MLCG base 32
    r ^= r << 7;     ///< xor-shift L
    r ^= r >> 13;
    r ^= r << 3;
    return r;
}

class hash_function {
    static constexpr int TABLE_SIZE = 256;
    array<uint32, TABLE_SIZE> table_;

  public:
    constexpr hash_function() {
        uint32 h = 2891336453; ///< good LCG multiplier
        for (auto a : range(0, TABLE_SIZE)) {
            for (auto b : range(0, 31)) {
                // scramble with xor-shift
                h ^= h >> 9;
                h ^= h << 17;
                h ^= h >> 6;
            }
            table_[a] = h;
        }
    }
    template <int N> constexpr uint32 operator()(const string<N> &p) const {
        uint32 h = 2024337845;
        for (auto a : p) {
            h = (h * 1372383749) ^ table_[static_cast<int>(a)];
        }
        return h;
    }
};

constexpr hash_function test_hash_function;
constexpr string<10> test_string{'h', 'e', 'l', 'l', 'o'};
static_assert(test_hash_function(test_string) == 1);

} // namespace rand
