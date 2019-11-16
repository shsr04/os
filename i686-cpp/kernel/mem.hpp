#pragma once
#include "core.hpp"
#include "initializer_list.hpp"
#include "term.hpp"

/**
 * Auto-referenced by clang for some big initializations.
 */
extern "C" void *memset(void *dest, int ch, uint32 count) {
    for (uint32 a = 0; a < count; a++)
        reinterpret_cast<uint8 *>(dest)[a] = static_cast<uint8>(ch);
    return dest;
}

namespace mem {
constexpr int KB = 1024;
constexpr int MB = 1024 * KB;

constexpr int HEAP_START = 2 * MB;
constexpr int HEAP_END = 22 * MB;

template <class T> class unique {
    T *value_;

  public:
    template <class... U> unique(U... p) : value_(new T(p...)) {}
    ~unique() { delete value_; }
};

template <int N> class bit_sequence {
    array<uint8, N / 8 + 1> bytes_ = {0};

    class bit_member {
        bit_sequence &b_;
        int pos_;
        bool val_;

      public:
        constexpr bit_member(bit_sequence &b, int pos)
            : b_(b), pos_(pos), val_(b[pos]) {}
        constexpr operator bool() const { return val_; }
        constexpr void operator=(bool p) {
            b_.bytes_[pos_ / 8] &= (p << (pos_ % 8));
        }
        constexpr int index() const { return pos_; }
    };

    class bit_iterator {
        bit_sequence &b_;
        int pos_;

      public:
        constexpr bit_iterator(bit_sequence &b, int pos = 0)
            : b_(b), pos_(pos) {}
        constexpr auto &operator++() {
            pos_++;
            return *this;
        }
        constexpr auto operator*() { return bit_member(b_, pos_); }
        constexpr auto operator!=(bit_iterator p) const {
            return pos_ != p.pos_;
        }
    };

  public:
    constexpr bit_sequence() {}
    template <class... U> constexpr bit_sequence(U... p) {
        for (auto a : {static_cast<int>(p)...}) {
            set(a);
        }
    }

    template <class I> constexpr bool operator[](I i) const {
        auto b = bytes_[i / 8];
        return (b & (1 << (i % 8))) != 0;
    }
    template <class I> constexpr bit_member operator[](I i) {
        return bit_member(*this, static_cast<int>(i));
    }
    constexpr auto begin() { return bit_iterator(*this); }
    constexpr auto end() { return bit_iterator(*this, N); }
    constexpr auto size() const { return N; }

    template <class I> constexpr void set(I i, bool x = true) {
        auto &b = bytes_[i / 8];
        if (x)
            b |= (1 << (i % 8));
        else
            b &= ~(1 << (i % 8));
    }

    constexpr bool is_empty() const {
        for (auto b : *this) {
            if (b)
                return false;
        }
        return true;
    }
};

constexpr bit_sequence<10> bit_test(2, 5, 7);
static_assert(bit_test[2] && bit_test[5] && bit_test[7]);
static_assert(!bit_test[1] && !bit_test[4] && !bit_test[6]);

class linked_list {
    struct node {
        int val;
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

    void push(int p) {
        if (head_ == nullptr) {
            head_ = new node{p};
            tail_ = head_;
            return;
        }
        tail_->next = new node{p};
    }

    optional<int> peek() {
        if (tail_ == nullptr)
            return {false, "List empty"};
        return tail_->val;
    }
};

} // namespace mem

namespace {

template <int BS, int BN> class allocator {
    int next_free = 0;
    mem::bit_sequence<BN> block_list;

  public:
    constexpr allocator() {
        static_assert(BS * BN < mem::HEAP_END - mem::HEAP_START,
                      "Not enough memory");
    }

    constexpr optional<int> find_next_free() const {
        for (int a = 0; a < BN; a++) {
            if (!block_list[a])
                return a;
        }
        return {};
    }

    constexpr optional<uint32 *> allocate(int blocks) {
        if (blocks >= BN - next_free || !find_next_free())
            return {false, "Not enough free memory"};
        auto r = reinterpret_cast<uint32 *>(mem::HEAP_START + BS * next_free);
        next_free = find_next_free().value;
        return r;
    }

    constexpr optional<bool> deallocate(void *p) {
        auto target = reinterpret_cast<uint32>(p) - mem::HEAP_START;
        if (!block_list[target]) {
            return {false, "Deallocating already free block"};
        }
        block_list.set(target, false);
        next_free = find_next_free().value;
        return true;
    }
};

allocator<4 * sizeof(uint32), 1000> fast_allocator;

} // namespace

void *operator new(uint32 count) {
    if (count >= 4 * sizeof(uint32)) {
        term::fatal_error("Cannot allocate more than ",
                          int_to_string(4 * sizeof(uint32)).str(),
                          " bytes at once\n");
    }
    auto r = fast_allocator.allocate(1);
    if (!r) {
        term::fatal_error("Allocation failed: ", r.err, "\n");
    }
    return r.value;
}

void operator delete(void *p) noexcept { fast_allocator.deallocate(p); }
