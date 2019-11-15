#pragma once

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(int) == 4);

#define DEF_MOVE(name, mode)                                                   \
    name(name &&) = mode;                                                      \
    name &operator=(name &&) = mode;
#define NO_COPY(name)                                                          \
    name(const name &) = delete;                                               \
    name &operator=(const name &) = delete;                                    \
    DEF_MOVE(name, default)

template <class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> constexpr T min(T a, T b) { return a < b ? a : b; }

template <class T> struct remove_ref_impl { using type = T; };
template <class T> struct remove_ref_impl<T &> { using type = T; };
template <class T> struct remove_ref_impl<T &&> { using type = T; };
template <class T> using remove_ref = typename remove_ref_impl<T>::type;

template <class T> constexpr auto move(T &&t) {
    return static_cast<remove_ref<T> &&>(t);
}

template <class T> class optional {
    bool loaded_ = false;

  public:
    const T value;
    const char *err = "(Unknown error)";

    constexpr optional() : value() {}
    constexpr optional(T p) : loaded_(true), value(move(p)) {}
    NO_COPY(optional)

    constexpr operator bool() const { return loaded_; }

    T or_get(T p) {
        if (!loaded_)
            return p;
        else
            return value;
    }
};

template <class T> class dual_iterator {
    int index_ = 0;
    T *ptr_;

    /// Access either the current index or value.
    struct dual_element {
        const int index;
        const T value;
        /// Automatic conversion to T
        constexpr operator T() const { return value; }
        /// If conversion not triggered, force with the function-call operator
        constexpr auto operator()() const { return value; }
    };

  public:
    constexpr dual_iterator(T *p) : ptr_(p) {}

    constexpr auto operator*() const { return dual_element{index_, *ptr_}; }
    constexpr auto operator++() {
        index_++;
        ptr_++;
        return *this;
    }
    constexpr auto operator!=(const dual_iterator<T> &p) {
        return ptr_ != p.ptr_;
    }
};

template <class T, int N> class array {
  protected:
    static_assert(N > 0, "Please do not create an empty array");
    T data_[N + 1];
    int index_ = 0;

  public:
    constexpr array() : data_{T()} {}
    template <class... U>
    constexpr array(U &&... p) : data_{static_cast<T>(p)...} {}
    NO_COPY(array)

    template <class I> constexpr auto &operator[](I i) { return data_[i]; }
    template <class I> constexpr auto operator[](I i) const { return data_[i]; }

    constexpr bool operator==(const array<T, N> &p) const {
        for (int a = 0; a < N; a++) {
            if (operator[](a) != p[a])
                return false;
        }
        return true;
    }
    template <int M> constexpr auto operator+(const array<T, M> &p) const {
        array<T, N + M> r(data_);
        for (int a = size(); a < p.size(); a++) {
            r[a] = p[a - size()];
        }
        return r;
    }

    constexpr auto clone() const {
        array<T, N> r;
        for (auto &&a : *this)
            r[a.index] = a;
        return r;
    }

    constexpr auto begin() const { return dual_iterator<const T>(data_); }
    constexpr auto end() const { return dual_iterator<const T>(data_ + N); }
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
template <int N> class string : public array<char, N> {
    using array<char, N>::array;

  public:
    using array<char, N>::operator==;
    constexpr bool operator==(const char *p) const {
        for (int a = 0; a < length(); a++) {
            if (p[a] == 0 || p[a] != this->operator[](a))
                return false;
        }
        return p[length()] == 0;
    }
    constexpr bool operator!=(const char *p) const { return !operator==(p); }
    /**
     * Create a string literal from this string object.
     */
    const char *str() const { return this->data_; }

    constexpr auto length() const {
        for (int a = 0; a < N; a++) {
            if (this->operator[](a) == 0)
                return a;
        }
        return N;
    }
    constexpr auto begin() const {
        return dual_iterator<const char>(this->data_);
    }
    constexpr auto end() const {
        return dual_iterator<const char>(this->data_ + length());
    }

    constexpr optional<string<N>> extract_word(int num,
                                               char separator = ' ') const {
        string<N> r;
        int seps = 0, i = 0;
        for (auto a : *this) {
            if (a == separator) {
                seps++;
            } else if (seps == num) {
                r[i++] = a();
                // r[a.index] = a(); // TODO fix
            }
        }
        if (seps < num)
            return {};
        return move(r);
    }
};

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

template <int A, int B> class range_impl {
    static_assert(B > A, "Cannot create an empty/inverse range");
    int a = A, b = B;

    class range_iterator {
        int value;

      public:
        range_iterator(int p) : value(p) {}
        constexpr auto operator*() const { return value; }
        constexpr auto operator++() {
            value++;
            return *this;
        }
        constexpr auto operator!=(const range_iterator &p) {
            return value != p.value;
        }
    };

  public:
    range_impl() = default;
    NO_COPY(range_impl)
    constexpr auto begin() const { return range_iterator(a); }
    constexpr auto end() const { return range_iterator(b); }
};

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

template <class R, class... P> using function = R (*)(P...);

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
