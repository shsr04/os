#if defined(__linux__)
#error "The kernel must target bare metal!"
#endif

#include "kernel/core.hpp"
#include "kernel/game_of_life.hpp"
#include "kernel/initializer_list.hpp"
#include "kernel/kbd.hpp"
#include "kernel/mem.hpp"
#include "kernel/multiboot.h"
#include "kernel/ps2.hpp"
#include "kernel/rand.hpp"
#include "kernel/term.hpp"
#include "kernel/time.hpp"

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

    auto mods = reinterpret_cast<multiboot_module_t *>(mb->mods_addr);
    if (mb->mods_count > 0) {
        term::write("Loaded ", int_to_string(mb->mods_count).str(),
                    " modules\n");
        for (uint32 a = 0; a < mb->mods_count; a++) {
            term::write("Module ", int_to_string(a).str(), ": from ",
                        int_to_string<16>(mods[a].mod_start).str(), " to ",
                        int_to_string<16>(mods[a].mod_end).str(), "\n");
            term::write(" ", reinterpret_cast<const char *>(mods[a].cmdline),
                        "\n");
        }
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
        rand::random_gen rnd;
        auto &&line = kbd::get_line();
        auto &&command = line.extract_word(0).value;
        auto &&param = line.extract_word(1).value;
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
            auto address = reinterpret_cast<uint32 *>(string_to_int<16>(param));
            term::write("-> 0x", int_to_string<16>(*address).str(), "\n");
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
            if (int mod_num; param != "" && (mod_num = string_to_int(param)) <
                                                int(mb->mods_count)) {
                auto mod = reinterpret_cast<int (*)(void)>(
                    mods[mod_num].mod_start + 0x1000);
                time::delay(100);
                term::write(">>> ", int_to_string<16>(mod()).str(), "\n");
            } else {
                term::write("Usage: mod <num>\n\tnum: module number ");
                if(mb->mods_count>0) term::write("(max: ",
                            int_to_string(mb->mods_count).str(), ")\n");
                else term::write("(none loaded)\n");
            }
        }

        if (command == "game") {
            game_of_life g;
            g.run();
        }
        if (command == "matrix") {
            auto n = param != "" ? string_to_int(param) : 1;
            term::clear();
            term::Term.set_colour(term::GREEN);
            term::Term.flipped = true;
            term::Term.wrap = true;
            for (int a = 0; a < n; a++) {
                for (auto _ : range<0, term::COLS>) {
                    (void)_;
                    for (auto _ : range<0, term::ROWS>) {
                        (void)_;
                        if (rnd.next(10) == 1) {
                            term::write('\n');
                            break;
                        }
                        term::write(char('$' + (rnd.next(60))));
                        time::delay(30);
                    }
                }
            }
            term::Term.flipped = false;
            term::Term.wrap = false;
            term::Term.set_colour(term::WHITE);
        }
        if (command == "coredump") {
            for (auto a : range<0, (term::ROWS + 2) * term::COLS>) {
                term::write(char('$' + (rnd.next(40))));
                time::delay(20);
            }
        }
    }

    term::write("Hello!\n");
}
