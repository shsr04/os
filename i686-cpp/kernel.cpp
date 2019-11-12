#if defined(__linux__)
#error "The kernel must target bare metal!"
#endif

#include "core.hpp"
#include "mem.hpp"
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
    linked_list l;
    l.push(520);
    term::write(int_to_string(l.peek().value, str), "\n");
    auto x = [] { term::write("x!"); };
    term::write("Hello!\n");
}
