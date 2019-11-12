#if defined(__linux__)
#error "The kernel must target bare metal!"
#endif

#include "core.hpp"
#include "mem.hpp"
#include "ps2.hpp"
#include "term.hpp"
#include <initializer_list.hpp>
#include <multiboot.h>

class linked_list {
    struct node {
        uint32 val;
        node *next = nullptr;
    } *head_ = nullptr, *tail_ = nullptr;

  public:
    linked_list() = default;
    ~linked_list() {
        while (head_ != tail_) {
            node *next = head_->next;
            delete head_;
            head_ = next;
        }
    }
    void push(uint32 p) {
        if (head_ == nullptr) {
            head_ = new node{p};
            tail_ = head_;
            return;
        }
        tail_->next = new node{p};
    }

    optional<uint32> peek() {
        if (tail_ == nullptr)
            return optional<uint32>().fail("List empty");
        return tail_->val;
    }
};

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

    ps2::disable_1();
    ps2::disable_2();
    ps2::flush_output();
    uint8 cb = ps2::read_config();
    term::write("PS/2: ");
    cb &= 0b10111100; // disable interrupts and translation
    ps2::write_config(cb);
    if (ps2::test_controller() && ps2::test_port_1())
        term::write("OK\n");
    else
        term::write("FAIL\n");
    ps2::enable_1();
    if(ps2::reset_1()) term::write("Device 1: OK");
    uint16 t = 0;
    while (!ps2::has_output_data()) ;
    term::write(int_to_string<16>(ps2::get_output(),str));

    auto x = [] { term::write("x!"); };
    term::write("Hello!\n");
}
