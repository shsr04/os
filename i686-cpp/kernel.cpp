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
    term::write("Loaded GRUB info: ", int_to_string<16>(magic).str(), "\n");

    if (mb->mods_count > 0) {
        term::write("Loaded ", int_to_string(mb->mods_count).str(),
                    " modules at 0x", int_to_string<16>(mb->mods_addr).str(),
                    "\n");
    }

    if ((mb->flags & 1) == 1) {
        term::write("Mem size: ",
                    int_to_string(mb->mem_upper * mem::KB / mem::MB).str(),
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
        auto command = line.extract_word(0).value;
        auto param = line.extract_word(1).value;
        if (command == "exit") {
            halt();
        }
        if (command == "reset") {
            ps2::hard_reset();
        }
        if (command == "clear") {
            term::clear();
        }
        if (command == "read") {
            auto address = reinterpret_cast<uint32 *>(string_to_int(param));
            term::write("-> ", int_to_string<16>(*address).str(), "\n");
        }
        if (command == "flip") {
            term::Term.flipped = !term::Term.flipped;
        }
        if (command == "stoi") {
            if (param != "") {
                term::write("non-empty ");
            }
            term::write("OK: '", param.str(), "'\n");
        }
        if (command == "mod") {
            auto mod = reinterpret_cast<void (*)(void)>(mb->mods_addr);
            term::write(">>> ");
            time::delay(100);
            mod();
        }

        if (command == "game") {
            game_of_life g;
            g.run();
        }
        if (command == "matrix") {
            rand::random_gen r;
            auto n = param != "" ? string_to_int(param) : 1;
            term::clear();
            term::Term.set_colour(term::GREEN);
            term::Term.flipped = true;
            for (int a = 0; a < n; a++) {
                for (auto _ : range<0, term::COLS>) {
                    (void)_;
                    for (auto _ : range<0, term::ROWS>) {
                        (void)_;
                        if (r.next(10) == 1) {
                            term::write('\n');
                            break;
                        }
                        term::write(char('$' + (r.next(60))));
                        time::delay(30);
                    }
                }
            }
            term::Term.flipped = false;
            term::Term.set_colour(term::WHITE);
        }
    }

    term::write("Hello!\n");
}
