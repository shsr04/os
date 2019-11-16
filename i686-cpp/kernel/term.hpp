#pragma once
#include "core.hpp"

namespace term {
constexpr int VGA_COLS = 80, VGA_ROWS = 25;
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

constexpr uint8 vga_colour(COLOURS fg, COLOURS bg = COLOURS::BLACK) {
    return static_cast<uint8>(fg | (bg << 4));
}

class terminal {
    void increment_cursor(int &main, int &cross, int main_limit,
                          int cross_limit, int times = 0) {
        for (int a = 0; a < times; a++) {
            main++;
            if (main == main_limit) {
                main = 0;
                cross++;
            }
            if (cross == cross_limit) {
                if (wrap) {
                    main = 0;
                    cross = 0;
                } else {
                    main = 0;
                    cross--;
                    for (auto a : range(0, VGA_ROWS * VGA_COLS)) {
                        auto coord = col_row(a);
                        if (coord._1 < max_col && coord._2 < max_row) {
                            if (a < (VGA_ROWS - 1) * VGA_COLS)
                                VGA_BUF[a] = VGA_BUF[a + VGA_COLS];
                            else
                                put(a, ' ');
                        }
                    }
                }
            }
        }
    }

    void put(int index, char symbol) {
        VGA_BUF[index] = static_cast<uint16>((colour << 8) | symbol);
    }

    constexpr auto index(int col, int row) const {
        return (VGA_COLS * row) + col;
    }

    constexpr pair<int, int> col_row(int index) const {
        return {index % VGA_COLS, index / VGA_COLS};
    }

    void advance(int n = 1) {
        if (!flipped) {
            increment_cursor(col, row, max_col, max_row, n);
        } else {
            increment_cursor(row, col, max_row, max_col, n);
        }
    }

    void new_line() {
        if (!flipped) {
            increment_cursor(col, row, max_col, max_row,
                             max_col - (col % max_col));
        } else {
            increment_cursor(row, col, max_row, max_col,
                             max_row - (row % max_row));
        }
    }

  public:
    const int max_col, max_row;
    int col = 0, row = 0;
    uint8 colour = COLOURS::WHITE;
    bool flipped = false, wrap = false;

    constexpr terminal(int cols = VGA_COLS, int rows = VGA_ROWS)
        : max_col(cols), max_row(rows) {}
    NO_SPECIAL(terminal)

    void clear() {
        for (int a = 0; a < max_col; a++) {
            for (int b = 0; b < max_row; b++) {
                put(index(a, b), ' ');
            }
        }
    }

    void write(char c) {
        switch (c) {
        case '\n':
            new_line();
            break;
        case '\t':
            advance(4);
            break;
        default:
            put(index(col, row), c);
            advance();
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
};

template <class... S>
[[noreturn]] void fatal_error(term::terminal &term, S... msg) {
    term.write(msg...);
    halt();
}

} // namespace term
