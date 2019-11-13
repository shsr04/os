#pragma once
#include "core.hpp"

namespace term {
constexpr int COLS = 80, ROWS = 25;
volatile uint16 *VGA_BUF = reinterpret_cast<uint16 *>(0xB8000);

enum colours : uint8 {
    // https://www.fountainware.com/EXPL/vga_color_palettes.htm
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15,
};

constexpr uint8 get_colour(colours fg, colours bg) { return fg | (bg << 4); }

struct {
    int col = 0, row = 0;
    uint8 colour = get_colour(WHITE, BLACK);
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
    case '\t':
        Term.col += 4;
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

void write(const char *s) {
    for (int a = 0; s[a] != 0; a++) {
        write(s[a]);
    }
}

void write(int p) { write(static_cast<char>('0' + p)); }

template <class... S> void write(S... p) {
    for (auto &a : {p...}) {
        write(a);
    }
}

template <class... S>[[noreturn]] void fatal_error(S... msg) {
    term::write(msg...);
    halt();
}

} // namespace term
