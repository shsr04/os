#if defined(__linux__) || !defined(__i386__)
#error "The kernel must target i386 bare metal!"
#endif

#include "core.hpp"
#include "initializer_list.hpp"
#include "multiboot.h"
#include "term.hpp"

namespace mem {
constexpr uint32 KB = 1024;
constexpr uint32 MB = 1024 * KB;

constexpr uint32 HEAP_START = 2 * MB;
constexpr uint32 HEAP_END = 22 * MB;

template <uint32 BS, uint32 BN> class allocator {
    uint32 next_free = 0;

  public:
    constexpr allocator() {
        static_assert(BS * BN < HEAP_END - HEAP_START, "Not enough memory");
    }

    constexpr optional<uint32 *> allocate(uint32 blocks) {
        if (blocks >= BN)
            return {};
        auto r = reinterpret_cast<uint32 *>(HEAP_START + BS * next_free);
        next_free += blocks;
        return r;
    }
    constexpr void deallocate(uint32 blocks) {
        if (blocks > next_free) {
            next_free=0;
            return;
        }
        next_free -= max(blocks,next_free);
    }
};

template <class T> class unique {
    T *value_;

  public:
    template <class... U> unique(U... p) : value_(new T(p...)) {}
    ~unique() { delete value_; }
};

} // namespace mem

namespace {
mem::allocator<4 * sizeof(uint32), 1000> fast_allocator;
}

void *operator new(uint32 count) {
    if (count >= 4 * sizeof(uint32)) {
        array<char, 10> str;
        term::write("Cannot allocate more than ",
                    int_to_string(4 * sizeof(uint32), str), " bytes at once\n");
        asm("hlt");
    }
    return fast_allocator.allocate(1).value;
}

void operator delete(void *) noexcept {
    fast_allocator.deallocate(1);
}

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
            return {};
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
    array<char, 20> str;
    term::write("Loaded GRUB info: ", int_to_string<16>(magic, str), "\n");
    if ((mb->flags & 1) == 1) {
        term::write(
            "Mem size: ", int_to_string(mb->mem_upper * mem::KB / mem::MB, str),
            " MB\n");
    }
    linked_list l;
    l.push(520);
    term::write(int_to_string(l.peek().value, str));
    auto x = [] { term::write("x!"); };
    term::write("Hello!\n");
}
