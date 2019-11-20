#pragma once
#include "core.hpp"
#include "rand.hpp"
#include "term.hpp"

class game_of_life {
    static constexpr int height=term::VGA_COLS, width=term::VGA_ROWS;
    bit_sequence<height * width> board_;
    bit_sequence<height * width> marked_;
    term::terminal &term;

  public:
    game_of_life(term::terminal &t) : term(t) {
        rand::random_gen rnd(34);
        for (int a = 0; a < 100; a++) {
            auto x = rnd.next() % width;
            auto y = rnd.next() % height;
            board_[width * y + x] = 1;
        }
    }

    void run(int gens = 10) {
        for (int a = 0; a < gens; a++) {
            step();
            draw();
            while (true)
                ;
        }
    }

    constexpr bool board(int x, int y) const { return board_[width * y + x]; }
    constexpr auto board(int x, int y) { return board_[width * y + x]; }

    void step() {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                auto c = board(x, y);
                int n = neighbours(x, y);
                if ((c == 1 && (n < 2 || n > 3)) || (c == 0 && n == 3))
                    mark(x, y);
            }
        }
        flip_board();
    }

    void mark(int x, int y) {
        marked_[width * y + x] = !marked_[width * y + x];
    }

    void flip_board() {
        for (int a = 0; a < board_.size(); a++) {
            board_[a] = bool(marked_[a]);
        }
    }

    void draw() {
        term.clear();
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                auto c = board(x, y);
                term.write(c ? 'X' : '-');
            }
        }
        // static int maxAlive = 0;
        // int alive = board.setBits();
        // maxAlive = max(alive, maxAlive);
        // screen.write(width + 1, 0, "Gen: " + str(gen));
        // screen.write(width + 1, 1, "Alive: " + str(alive));
        // screen.write(width + 1, 2, "Max alive: " + str(maxAlive));
        // screen.refresh();
    }

    int neighbours(int x, int y) const {
        int r = 0;
        if (x > 0) {
            r += board(x - 1, y);
            if (y > 0)
                r += board(x - 1, y - 1);
            if (y < height - 1)
                r += board(x - 1, y + 1);
        }
        if (x < width - 1) {
            r += board(x + 1, y);
            if (y > 0)
                r += board(x + 1, y - 1);
            if (y < height - 1)
                r += board(x + 1, y + 1);
        }
        if (y > 0) {
            r += board(x, y - 1);
        }
        if (y < height - 1) {
            r += board(x, y + 1);
        }
        return r;
    }
};
