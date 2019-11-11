#pragma once

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
class uint64 {
    uint32 low, high;

  public:
    uint64() = default;
    uint64(uint32 l, uint32 h) : low(l), high(h) {}
    auto operator=(uint32 x) {
        low = x;
        high = 0;
    }
    uint64 operator+(uint64 &x) { return {low + x.low, high + x.high}; }
    operator uint32() { return low; }
    operator uint16() { return static_cast<uint8>(low); }
    operator uint8() { return static_cast<uint8>(low); }
};

template <class T, uint32 N> using array = T[N];

constexpr uint32 digits(uint32 p) {
    uint32 r = 0;
    while (p / 10 > 0) {
        r++;
        p /= 10;
    }
    return r;
}

template <uint32 N>
constexpr auto int_to_string(uint32 x, array<char, N> &r) {
    for (int a = 0; x > 0; a++) {
        r[a] = '0' + (x % 10);
        x /= 10;
        if (x == 0)
            r[a] = 0;
    }
    return r;
}
