#if defined(__linux__) || !defined(__i386__)
#error "The kernel must target i386 bare metal!"
#endif

#include "core.hpp"
#include "multiboot.h"

namespace mem {
constexpr uint32 START = 2 * 1024 * 1024;
uint32 END;
} // namespace mem

namespace term {
constexpr int COLS = 80, ROWS = 25;
volatile uint16 *VGA_BUF = reinterpret_cast<uint16 *>(0xb8000);

struct {
    int col = 0, row = 0;
    uint8 colour = 0x0f;
} Term;

constexpr auto index(int col = Term.col, int row = Term.row) {
    return (COLS * row) + col;
}

void set(int index, uint8 colour, char symbol) {
    VGA_BUF[index] = static_cast<uint16>((colour << 8) | symbol);
}

void clear() {
    for (int a = 0; a < COLS; a++) {
        for (int b = 0; b < ROWS; b++) {
            set(index(a, b), Term.colour, ' ');
        }
    }
}

void write(char c) {
    switch (c) {
    case '\n':
        Term.col = 0;
        Term.row++;
        break;
    default:
        set(index(), Term.colour, c);
        Term.col++;
        break;
    }
    if (Term.col >= COLS) {
        Term.col = 0;
        Term.row++;
    }
    if (Term.row >= ROWS) {
        Term.col = 0;
        Term.row = 0;
    }
}

void write(const char *const s) {
    for (int a = 0; s[a] != 0; a++) {
        write(s[a]);
    }
}

template <uint32 N> void write(array<const char *const, N> s) {
    for (auto a : s)
        write(a);
}

} // namespace term

template <class F> class Runner {
    F f_;

  public:
    Runner(F f) : f_(f) { term::write("Runner started\n"); }
    ~Runner() {
        f_();
        term::write("Runner ended\n");
    }
};

extern "C" void kernel_main(multiboot_info_t *mb, uint32 magic) {
    term::clear();
    array<char, 10> str;
    term::write("Loading... ");
    term::write(int_to_string(magic, str));
    term::write("\n");
    if ((mb->flags & 1) == 1) {
        mem::END = mem::START + (mb->mem_upper - 1) * 1024;
        term::write("Mem areas loaded\n");
        term::write("Mem size: ");
        term::write(int_to_string(mem::END - mem::START, str));
        term::write(" B\n");
    }
    auto x = [] { term::write("x!"); };
    term::write("Hello!\n");
}
