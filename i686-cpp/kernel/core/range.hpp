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
