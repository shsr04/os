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

template <int N> class bit_sequence {
    array<uint8, N / 8 + 1> bytes_ = {0};

    class bit_member {
        bit_sequence &b_;
        int pos_;
        bool val_;

      public:
        constexpr bit_member(bit_sequence &b, int pos)
            : b_(b), pos_(pos), val_(b[pos]) {}
        constexpr operator bool() const { return val_; }
        constexpr void operator=(bool p) {
            b_.bytes_[pos_ / 8] &= (p << (pos_ % 8));
        }
        constexpr int index() const { return pos_; }
    };

    class bit_iterator {
        bit_sequence &b_;
        int pos_;

      public:
        constexpr bit_iterator(bit_sequence &b, int pos = 0)
            : b_(b), pos_(pos) {}
        constexpr auto &operator++() {
            pos_++;
            return *this;
        }
        constexpr bit_member operator*() { return b_[pos_]; }
        constexpr auto operator!=(bit_iterator p) const {
            return pos_ != p.pos_;
        }
    };

  public:
    constexpr bit_sequence() {}
    template <class... U> constexpr bit_sequence(U... p) {
        for (auto a : {static_cast<int>(p)...}) {
            set(a);
        }
    }

    template <class I> constexpr bool operator[](I i) const {
        auto b = bytes_[i / 8];
        return (b & (1 << (i % 8))) != 0;
    }
    template <class I> constexpr bit_member operator[](I i) {
        return bit_member(*this, static_cast<int>(i));
    }
    constexpr auto begin() { return bit_iterator(*this); }
    constexpr auto end() { return bit_iterator(*this, N); }
    constexpr auto size() const { return N; }

    template <class I> constexpr void set(I i, bool x = true) {
        auto &b = bytes_[i / 8];
        if (x)
            b |= (1 << (i % 8));
        else
            b &= ~(1 << (i % 8));
    }

    constexpr bool is_empty() const {
        for (auto b : *this) {
            if (b)
                return false;
        }
        return true;
    }
};

constexpr bit_sequence<10> bit_test(2, 5, 7);
static_assert(bit_test[2] && bit_test[5] && bit_test[7]);
static_assert(!bit_test[1] && !bit_test[4] && !bit_test[6]);
