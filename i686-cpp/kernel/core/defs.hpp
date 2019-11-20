using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

static_assert(sizeof(uint8) == 1);
static_assert(sizeof(uint16) == 2);
static_assert(sizeof(uint32) == 4);
static_assert(sizeof(int) == 4);

constexpr int KB = 1024;
constexpr int MB = 1024 * KB;

template <class R, class... P> using function = R (*)(P...);

#define DEF_COPY(name, mode)                                                   \
    name(const name &) = mode;                                                 \
    name &operator=(const name &) = mode;
#define DEF_MOVE(name, mode)                                                   \
    name(name &&) = mode;                                                      \
    name &operator=(name &&) = mode;
#define NO_COPY(name) DEF_COPY(name, delete) DEF_MOVE(name, default)
#define NO_SPECIAL(name) DEF_COPY(name, delete) DEF_MOVE(name, delete)

template <class T> constexpr T max(T a, T b) { return a > b ? a : b; }
template <class T> constexpr T min(T a, T b) { return a < b ? a : b; }
template <class T> constexpr T abs(T a) { return a >= 0 ? a : -a; }

template <class T> struct remove_ref_impl { using type = T; };
template <class T> struct remove_ref_impl<T &> { using type = T; };
template <class T> struct remove_ref_impl<T &&> { using type = T; };
template <class T> using remove_ref = typename remove_ref_impl<T>::type;

template <class T> constexpr auto move(T &&t) {
    return static_cast<remove_ref<T> &&>(t);
}

/// Auto-referenced by clang for some big initializations.
extern "C" void *memset(void *dest, int ch, uint32 count) {
    for (uint32 a = 0; a < count; a++)
        reinterpret_cast<uint8 *>(dest)[a] = static_cast<uint8>(ch);
    return dest;
}
extern "C" void *memcpy(void *dest, const void *src, uint32 count) {
    for (uint32 a = 0; a < count; a++)
        reinterpret_cast<uint8 *>(dest)[a] =
            *reinterpret_cast<const uint8 *>(src);
    return dest;
}
