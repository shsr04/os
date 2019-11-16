using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(int) == 4);

template <class R, class... P> using function = R (*)(P...);

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
