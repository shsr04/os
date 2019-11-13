#pragma once
#include "core.hpp"
#include "term.hpp"
#include <initializer_list.hpp>

namespace mem {
constexpr uint32 KB = 1024;
constexpr uint32 MB = 1024 * KB;

constexpr uint32 HEAP_START = 2 * MB;
constexpr uint32 HEAP_END = 22 * MB;

template <class T> class unique {
    T *value_;

  public:
    template <class... U> unique(U... p) : value_(new T(p...)) {}
    ~unique() { delete value_; }
};

template <uint32 N> class bit_sequence {
    array<uint8, N / 8 + 1> bytes_ = {0};

    class bit_member {
        bool val_;
        uint32 pos_;

      public:
        constexpr bit_member(bool val, uint32 pos) : val_(val), pos_(pos) {}
        constexpr operator bool() const { return val_; }
        constexpr uint32 index() const { return pos_; }
    };

    class bit_iterator {
        const bit_sequence &b_;
        uint32 pos_;

      public:
        constexpr bit_iterator(const bit_sequence &b, uint32 pos = 0)
            : b_(b), pos_(pos) {}
        constexpr auto &operator++() {
            pos_++;
            return *this;
        }
        constexpr auto operator*() const { return bit_member(b_[pos_], pos_); }
        constexpr auto operator!=(bit_iterator p) const {
            return pos_ != p.pos_;
        }
    };

  public:
    constexpr bit_sequence() {}
    template <class... U> constexpr bit_sequence(U... p) {
        for (auto a : {static_cast<uint32>(p)...}) {
            set(a);
        }
    }

    constexpr bool operator[](uint32 i) const {
        auto b = bytes_[i / 8];
        return (b & (1 << (i % 8))) != 0;
    }
    constexpr auto begin() const { return bit_iterator(*this); }
    constexpr auto end() const { return bit_iterator(*this, N); }
    constexpr auto size() const { return N; }

    constexpr void set(uint32 i, bool x = true) {
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

static_assert(bit_sequence<5>().is_empty());
constexpr bit_sequence<10> bit_test(2, 5, 7);
static_assert(!bit_test.is_empty());
static_assert(bit_test[2] && bit_test[5] && bit_test[7]);
static_assert(!bit_test[1] && !bit_test[4] && !bit_test[6]);

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
            return fail<uint32>("List empty");
        return tail_->val;
    }
};

} // namespace mem

namespace {

template <uint32 BS, uint32 BN> class allocator {
    uint32 next_free = 0;
    mem::bit_sequence<BN> block_list;

  public:
    constexpr allocator() {
        static_assert(BS * BN < mem::HEAP_END - mem::HEAP_START,
                      "Not enough memory");
    }

    constexpr optional<uint32> find_next_free() const {
        for (auto b : block_list) {
            if (!b)
                return b.index();
        }
        return {};
    }

    constexpr optional<uint32 *> allocate(uint32 blocks) {
        if (blocks >= BN - next_free || !find_next_free())
            return fail<uint32 *>("Not enough free memory");
        auto r = reinterpret_cast<uint32 *>(mem::HEAP_START + BS * next_free);
        next_free = find_next_free().value;
        return r;
    }

    constexpr optional<bool> deallocate(void *p) {
        auto target = reinterpret_cast<uint32>(p) - mem::HEAP_START;
        if (!block_list[target]) {
            return fail<bool>("Deallocating already free block");
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
        string<10> str;
        term::fatal_error("Cannot allocate more than ",
                          int_to_string(4 * sizeof(uint32), str),
                          " bytes at once\n");
    }
    auto r = fast_allocator.allocate(1);
    if (!r) {
        term::fatal_error("Allocation failed: ", r.err, "\n");
    }
    return r.value;
}

void operator delete(void *p) noexcept { fast_allocator.deallocate(p); }
