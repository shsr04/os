#if defined(__linux__)
#error "The kernel must target bare metal!"
#endif

#include "core.hpp"
#include "game_of_life.hpp"
#include "kbd.hpp"
#include "mem.hpp"
#include "ps2.hpp"
#include "rand.hpp"
#include "term.hpp"
#include "time.hpp"
#include <initializer_list.hpp>
#include <multiboot.h>

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
    string<20> str;
    term::write("Loaded GRUB info: ", int_to_string<16>(magic, str), "\n");
    if ((mb->flags & 1) == 1) {
        term::write(
            "Mem size: ", int_to_string(mb->mem_upper * mem::KB / mem::MB, str),
            " MB\n");
    }

    term::write("PS/2: ");
    if (ps2::startup())
        term::write("OK\n");
    else
        term::fatal_error("FAIL (shutting down)\n");

    while (true) {
        term::write(">  ");
        auto &line = kbd::get_line();
        if (line == "exit") {
            halt();
        }
        if(line=="clear") {
            term::clear();
        }

        if (line == "game") {
            game_of_life g;
            g.run();
        }
        if (line == "matrix") {
            rand::random_gen r;
            term::clear();
            term::Term.colour=term::GREEN;
            for (int a = 0; a < term::COLS * term::ROWS; a++) {
                term::write(char('$' + (r.next() % 60)));
                time::delay(30);
            }
            term::Term.colour=term::WHITE;
        }
    }

    term::write("Hello!\n");
}
