class range {
    int a, b;

    class range_iterator {
        int value;

      public:
        constexpr range_iterator(int p) : value(p) {}
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
    constexpr range(int p1, int p2) : a(min(p1, p2)), b(max(p1, p2)) {}
    NO_COPY(range)
    constexpr auto begin() const { return range_iterator(a); }
    constexpr auto end() const { return range_iterator(b); }
};

namespace {
constexpr range test_range(5, 10);
static_assert(*test_range.begin() == 5);
static_assert(*(++(++(++test_range.begin()))) == 8);
static_assert(*(++(++(++(++(++test_range.begin()))))) == *test_range.end());
} // namespace
