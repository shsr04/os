#pragma once

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(int) == 4);

template <class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> constexpr T min(T a, T b) { return a < b ? a : b; }

template <class T> class optional {
    bool loaded_ = false;

  public:
    const T value{};
    const char *err = "(Unknown error)";

    constexpr optional() = default;
    constexpr optional(T p) : loaded_(true), value(p) {}

    constexpr operator bool() const { return loaded_; }

    T or_get(T p) {
        if (!loaded_)
            return p;
        else
            return value;
    }
};

template <class T, uint32 N> class array {
  protected:
    static_assert(N > 0, "Please do not create an empty array");
    T data_[N];
    int length_ = 0;
    int index_ = 0;

  public:
    constexpr array() : data_{} {}
    template <class... U>
    constexpr array(U &&... p)
        : data_{static_cast<T>(p)...}, length_(sizeof...(p)) {}

    template <class I> constexpr auto &operator[](I i) { return data_[i]; }
    template <class I> constexpr auto operator[](I i) const { return data_[i]; }

    /// Get the array's internal index. (can be freely used for convenience)
    constexpr auto &index() { return index_; }
    /// Get the element at the internal index.
    constexpr auto &operator*() { return data_[index_]; }
    /// Increment the internal index.
    constexpr auto operator++() { index_++; }

    template <uint32 M> constexpr bool operator==(const array<T, M> &p) const {
        for (uint32 a = 0; a < min(N, M); a++) {
            if (operator[](a) != p[a])
                return false;
        }
        return true;
    }
    template <uint32 M> constexpr auto operator+(const array<T, M> &p) const {
        array<T, N + M> r(data_);
        for (int a = size(); a < p.size(); a++) {
            r[a] = p[a - size()];
        }
        return r;
    }

    constexpr auto begin() const { return data_; }
    constexpr auto end() const { return data_ + N; }
    constexpr auto size() const { return static_cast<int>(N); }

    auto count(T x) const {
        int r = 0;
        for (auto &a : data_) {
            if (a == x)
                r++;
        }
        return r;
    }
};
template <uint32 N> class string : public array<char, N> {
    using array<char, N>::array;

  public:
    using array<char, N>::operator==;
    constexpr bool operator==(const char *p) const {
        for (auto a = 0; a < this->size(); a++) {
            if (p[a] == 0)
                return true;
            else if (p[a] != this->operator[](a))
                return false;
        }
        return true;
    }
    constexpr bool operator!=(const char *p) const { return !operator==(p); }
    /**
     * Create a string literal from this string object.
     */
    const char *str() const { return this->data_; }

    constexpr auto begin() const { return this->data_; }
    constexpr auto end() const { return this->data_ + this->length_; }

    template <int I> optional<string<N>> extract_word(char separator) const {
        string<N> r;
        int seps = 0;
        for (auto &a : this->data_) {
            if (a == separator) {
                seps++;
            } else if (seps == I) {
                *r = a;
                ++r;
            }
        }
        if (seps < I)
            return {};
        *r = 0;
        return r;
    }
};

template <class T, class U> class pair {
  public:
    T _1;
    U _2;

    pair() = default;
    pair(T p1, U p2) : _1(p1), _2(p2) {}
    pair(pair<T, U> const &) = delete;
};

template <int A, int B> constexpr auto range_impl() {
    static_assert(B > A, "Cannot create an empty/inverse range");
    array<int, B - A> r;
    for (int a = A; a < B; a++) {
        r[a - A] = a;
    }
    return r;
}

template <int A, int B> constexpr auto range = range_impl<A, B>();

template <bool V> struct bi_state {
    static constexpr bool value = V;
    constexpr operator bool() const { return value; }
};
template <class T, class U> struct is_same_impl {
    using type = bi_state<false>;
};
template <class T> struct is_same_impl<T, T> { using type = bi_state<true>; };
template <class T, class U> using is_same = typename is_same_impl<T, U>::type;

template <class T> constexpr optional<T> fail(const char *p) {
    auto r = optional<T>();
    r.err = p;
    return r;
}

constexpr uint32 digits(uint32 p) {
    uint32 r = 0;
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
template <uint32 N> constexpr auto string_to_int(const string<N> &p) {
    int r = 0;
    for (auto c : p) {
        r *= 10;
        r += static_cast<int>(c - '0');
    }
    return r;
}

namespace {
constexpr string<10> test_string{'9', '8', '7'};
static_assert(string_to_int(test_string) == 987);
static_assert(int_to_string(15500) == "15500");
static_assert(int_to_string(string_to_int(test_string)) == test_string);
} // namespace

[[noreturn]] void halt() {
    asm("hlt");
    while (true)
        ;
}
