#if defined(__linux__) || !defined(__i386__)
#error "The kernel must target x86 bare metal!"
#endif

#include "category.hpp"
#include "kernel/core.hpp"
#include "kernel/game_of_life.hpp"
#include "kernel/initializer_list.hpp"
#include "kernel/kbd.hpp"
#include "kernel/matrix.hpp"
#include "kernel/mod.hpp"
#include "kernel/multiboot.h"
#include "kernel/ps2.hpp"
#include "kernel/rand.hpp"
#include "kernel/term.hpp"
#include "kernel/time.hpp"

extern "C" void kernel_main(multiboot_info_t *mb, uint32 magic, uint32 seed) {
    term::terminal term(term::VGA_COLS, term::VGA_ROWS);
    term.clear();
    term.write("Loaded GRUB info: ", int_to_string<16>(magic).str(), "\n");

    mod::module_list mods(*mb);
    if (mods.size() > 0) {
        term.write("Loaded ", int_to_string(mods.size()).str(), " modules\n");
        for (auto &&a : mods) {
            term.write("Module ", int_to_string(a.index).str(), ": from ",
                       int_to_string<16>(a().mod_start).str(), " to ",
                       int_to_string<16>(a().mod_end).str(), "\n");
            term.write(" ", reinterpret_cast<const char *>(a().cmdline), "\n");
        }
    }

    if ((mb->flags & 1) == 1) {
        term.write("Mem size: ", int_to_string(mb->mem_upper * KB / MB).str(),
                   " MB\n");
    }

    term.write("PS/2: ");
    if (ps2::startup())
        term.write("OK\n");
    else
        term::fatal_error(term, "FAIL (shutting down)\n");

    rand::random_gen rnd(seed);
    while (true) {
        term.write(">  ");
        auto &&line = kbd::get_line(term);
        auto &&command = line.extract_word(0).value;
        auto &&param = line.extract_word(1).value;
        if (command == "exit") {
            halt();
        }
        if (command == "reset") {
            ps2::hard_reset();
        }
        if (command == "clear") {
            term.clear();
        }
        if (command == "read") {
            auto address = reinterpret_cast<uint32 *>(string_to_int<16>(param));
            term.write("-> 0x", int_to_string<16>(*address).str(), "\n");
        }
        if (command == "flip") {
            term.flipped = !term.flipped;
        }
        if (command == "stoi") {
            if (param != "") {
                term.write("non-empty ");
            }
            term.write("OK: '", param.str(), "'\n");
        }
        if (command == "mod") {
            if (param != "" && mods.entry_point(string_to_int(param))) {
                auto proc = mods.entry_point(string_to_int(param)).value;
                time::delay(100);
                term.write(">>> ", int_to_string<16>(proc()).str(), "\n");
            } else {
                term.write("Usage: mod <num>\n\tnum: module number ");
                if (mb->mods_count > 0)
                    term.write("(max: ", int_to_string(mb->mods_count).str(),
                               ")\n");
                else
                    term.write("(none loaded)\n");
            }
        }

        if (command == "game") {
            game_of_life g(term);
            g.run();
        }
        if (command == "matrix") {
            auto n = param != "" ? string_to_int(param) : 1;
            term.colour = term::vga_colour(term::GREEN);
            term.flipped = true;
            term.wrap = true;
            for (int a = 0; a < n; a++) {
                term.clear();
                for (auto _ : range(0, term.max_col)) {
                    (void)_;
                    for (auto _ : range(0, term.max_row)) {
                        (void)_;
                        if (rnd.next(10) == 1) {
                            term.write('\n');
                            break;
                        }
                        term.write(char(' ' + (rnd.next(64))));
                        time::delay(30);
                    }
                }
            }
            term.flipped = false;
            term.wrap = false;
            term.colour = term::vga_colour(term::WHITE);
        }
        if (command == "coredump") {
            for (auto a : range(0, (term.max_row + 2) * term.max_col)) {
                (void)a;
                term.write(char('$' + (rnd.next(40))));
                time::delay(20);
            }
        }
    }

    term.write("Hello!\n");
}
