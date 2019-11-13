#pragma once

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);

template <class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> constexpr T min(T a, T b) { return a < b ? a : b; }

template <class T, uint32 N> class array {
  protected:
    static_assert(N > 0, "Please do not create an empty array");
    T data_[N];
    int index_ = 0;

  public:
    constexpr array() : data_{} {}
    template <class... U>
    constexpr array(U&&... p) : data_{static_cast<T>(p)...} {}
    //constexpr array(T p[N]) : data_(p) {}

    constexpr auto &operator[](int i) { return data_[i]; }
    constexpr auto operator[](int i) const { return data_[i]; }

    constexpr auto &index() { return index_; }
    constexpr auto &operator*() { return data_[index_]; }
    constexpr auto operator++() { index_++; }

    template <uint32 M> constexpr bool operator==(const array<T, M> &p) const {
        for (int a = 0; a < min(N, M); a++) {
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

    // constexpr operator T *() { return data_; }
};
template <uint32 N> class string : public array<char, N> {
  public:
    constexpr bool operator==(const char *p) const {
        for (auto a = 0; a < this->size(); a++) {
            if (p[a] == 0)
                return true;
            else if (p[a] != this->operator[](a))
                return false;
        }
        return true;
    }
    /**
     * Create a string literal from this string object.
     */
    const char *str() const { return this->data_; }
};

template <class T, class U> class pair {
  public:
    T _1;
    U _2;

    pair() = default;
    pair(T p1, U p2) : _1(p1), _2(p2) {}
    pair(pair<T, U> const &) = delete;
};

template <bool V> struct bi_state {
    static constexpr bool value = V;
    constexpr operator bool() const { return value; }
};
template <class T, class U> struct is_same_impl {
    using type = bi_state<false>;
};
template <class T> struct is_same_impl<T, T> { using type = bi_state<true>; };
template <class T, class U> using is_same = typename is_same_impl<T, U>::type;

template <class T> class optional {
    bool loaded_ = false;

  public:
    const T value{};
    const char *err = "(Unknown error)";

    constexpr optional() = default;
    constexpr optional(T p) : loaded_(true), value(p) {}

    constexpr operator bool() const { return loaded_; }
};

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

template <int B = 10, uint32 N>
constexpr const char *int_to_string(uint32 x, string<N> &r) {
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
    return r.str();
}

[[noreturn]] void halt() {
    asm("hlt");
    while (true)
        ;
}
