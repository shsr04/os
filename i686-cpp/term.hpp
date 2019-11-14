#pragma once
#include "core.hpp"

namespace term {
constexpr int COLS = 80, ROWS = 25;
volatile uint16 *VGA_BUF = reinterpret_cast<uint16 *>(0xB8000);

enum COLOURS : uint8 {
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

enum class direction { HOR, VERT };

class {
    void increment_cursor(int &main, int &cross, int main_limit,
                          int cross_limit, int times = 0) {
        for (int a = 0; a < times; a++) {
            main++;
            if (main >= main_limit) {
                main = 0;
                cross++;
            }
            if (cross >= cross_limit) {
                main = 0;
                cross = 0;
            }
        }
    }

  public:
    int col = 0, row = 0;
    uint8 colour = COLOURS::WHITE;
    bool flipped = false;
    void set_colour(COLOURS fg, COLOURS bg = COLOURS::BLACK) {
        colour = static_cast<uint8>(fg | (bg << 4));
    }
    void advance(int n = 1, direction dir = direction::HOR,
                 bool carriage_return = false) {
        if ((!flipped && dir == direction::HOR) ||
            (flipped && dir == direction::VERT)) {
            increment_cursor(col, row, COLS, ROWS, n);
            if (carriage_return)
                row = 0;
        } else {
            increment_cursor(row, col, ROWS, COLS, n);
            if (carriage_return)
                col = 0;
        }
    }
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
        Term.advance(1, direction::VERT, true);
        break;
    case '\t':
        Term.advance(4);
        break;
    default:
        set(index(), Term.colour, c);
        Term.advance();
        break;
    }
}

void write(const char *s) {
    for (int a = 0; s[a] != 0; a++) {
        write(s[a]);
    }
}

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
