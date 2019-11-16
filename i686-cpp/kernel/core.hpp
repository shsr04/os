#pragma once

#include "core/defs.hpp"

#include "core/array.hpp"
#include "core/optional.hpp"
#include "core/range.hpp"
#include "core/string.hpp"

class merge_sort {
    template <class T, int N>
    static constexpr void merge(array<T, N> &a, int i1, int im, int i2) {
        array<T, N> c;
        int offset = i1;
        int ci = 0;
        while (i1 <= im && im <= i2) {
            if (a[i1] > a[i2])
                c[ci++] = a[im++];
            else
                c[ci++] = a[i1++];
        }
        while (i1 <= im)
            c[ci++] = a[i1++];
        while (im <= i2)
            c[ci++] = a[im++];
        for (int d = 0; d < ci; d++) {
            a[offset + d] = c[d];
        }
    }

  public:
    template <class T, int N> static constexpr auto sort(const array<T, N> &a) {
        array<T, N> b = a.clone();
        for (int m = 1; m < N; m *= 2) {
            for (int i = 0; i < N - m; i += 2 * m) {
                merge(b, i, m, min(i + 2 * m - 1, N - 1));
            }
        }
        return b;
    }
};

template <class T, class U> class pair {
  public:
    T _1;
    U _2;

    pair() = default;
    pair(T p1, U p2) : _1(p1), _2(p2) {}
    NO_COPY(pair)
};

constexpr int digits(int p) {
    int r = 0;
    while (p / 10 > 0) {
        r++;
        p /= 10;
    }
    return r;
}

/// Convert an integer x to a number string "x"
template <int B = 10, class I> constexpr auto int_to_string(I x) {
    string<30> r;
    static_assert(B == 2 || B == 10 || B == 16);
    const auto i2c = [](auto x) -> char {
        if ((x % B) < 10)
            return '0' + (x % B);
        else
            return 'A' + ((x - 10) % B);
    };
    auto a = 0;
    if (x == 0)
        r[a++] = '0';
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

/// Convert a number string "x" to an integer x
template <int B = 10, int N> constexpr auto string_to_int(const string<N> &p) {
    int r = 0;
    for (auto c : p) {
        r *= B;
        r += static_cast<int>(c.value - '0');
    }
    return r;
}

template <class T> class action {
    function<T> f_;

  public:
    action() = default;
    action(function<T> f) : f_(f) {}

    template <class U> constexpr auto bind(action<U> p) {
        return action([this, &p] {
            f_();
            p.f_();
        });
    }
};

namespace {
constexpr array<int, 10> test_array{5, 4, 3, 1, 5, 20, 3, 17, 9, 1, 2};
static_assert(test_array.size() == 10);
constexpr auto test_iterator = *test_array.begin();
static_assert(test_iterator.index == 0 && test_iterator == 5);
// static_assert(merge_sort::sort(test_array) ==
//              array<int, 10>{1, 1, 2, 3, 3, 4, 5, 5, 9, 17, 20});

constexpr string<10> test_string{'9', '8', '7'};
static_assert(test_string.length() == 3);
static_assert(test_string != "98" && test_string == "987" &&
              test_string != "9876" && "987" == test_string);
static_assert(string_to_int(test_string) == 987);
static_assert(int_to_string(15500) == "15500");
static_assert(int_to_string(string_to_int(test_string)) == "987");
constexpr string<10> test_string2{'x', 'y', ' ', 'z', ' ', '-'};
static_assert(test_string2.length() == 6);
constexpr auto test_iterator2 = *(++(++(++test_string2.begin())));
static_assert(test_iterator2 == 'z' && test_iterator2.index == 3);
static_assert(test_string2.extract_word(0).value == "xy" &&
              test_string2.extract_word(1) &&
              test_string2.extract_word(1).value == "z");
} // namespace

[[noreturn]] void halt() {
    asm("hlt");
    while (true)
        ;
}
