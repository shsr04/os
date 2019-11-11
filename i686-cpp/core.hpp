#pragma once

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);

template <class T, uint32 N> using array = T[N];

template <class T> class optional {
    bool loaded_ = false;

  public:
    const T value{};

    constexpr optional() = default;
    constexpr optional(T p) : value(p) {}
    operator bool() { return loaded_; }
};

constexpr uint32 digits(uint32 p) {
    uint32 r = 0;
    while (p / 10 > 0) {
        r++;
        p /= 10;
    }
    return r;
}

template <int B = 10, uint32 N>
constexpr const char *int_to_string(uint32 x, array<char, N> &r) {
    static_assert(B == 10 || B == 16);
    const auto i2c = [](auto x) -> char {
        if ((x % B) < 10)
            return '0' + (x % B);
        else
            return 'A' + ((x - 10) % B);
    };
    auto a = 0;
    for (; x > 0; a++) {
        r[a] = i2c(x);
        x /= B;
    }
    r[a] = 0;
    for (auto b = 0; b < a / 2; b++) {
        r[b] ^= r[a - b - 1];
        r[a - b - 1] ^= r[b];
        r[b] ^= r[a - b - 1];
    }
    return r;
}

template <class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> constexpr T min(T a, T b) { return a < b ? a : b; }