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
