template <class T> class optional {
    bool loaded_ = false;

  public:
    const T value;
    const char *const err = "(Unknown error)";

    constexpr optional() : value() {}
    constexpr optional(T p) : loaded_(true), value(move(p)) {}
    constexpr optional(bool, const char *p) : loaded_(false), value(), err(p) {}
    NO_COPY(optional)

    constexpr operator bool() const { return loaded_; }

    T or_val(T p) {
        if (!loaded_)
            return p;
        else
            return value;
    }

    template <class F>
    constexpr auto map(F &&f) const -> optional<decltype(f(value))> {
        if (!loaded_)
            return {};
        else
            return f(value);
    }
};

namespace {
constexpr optional<int> test_optional{5};
static_assert(test_optional.map([](int t) { return 2 * t; }).value == 10);
static_assert(test_optional
                  .map([](int t) {
                      if (t < 5)
                          return t + 1;
                      else
                          return t - 1;
                  })
                  .value == 4);
} // namespace
