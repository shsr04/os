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
    array<uint8, N / 8 + 1> bytes_{0};

  public:
    constexpr bit_sequence() = default;
    constexpr bit_sequence(std::initializer_list<uint32> p) {
        for (auto a : p) {
            set(a);
        }
    }
    constexpr bool operator[](uint32 i) const {
        auto b = bytes_[i / 8];
        return (b & (1 << (i % 8))) != 0;
    }
    constexpr void set(uint32 i, bool x = true) {
        auto &b = bytes_[i / 8];
        if (x)
            b |= (1 << (i % 8));
        else
            b &= ~(1 << (i % 8));
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
        for (auto a = 0; a < BN; a++) {
            if (!block_list[a])
                return a;
        }
        return {};
    }

    constexpr optional<uint32 *> allocate(uint32 blocks) {
        if (blocks >= BN || !find_next_free())
            return optional<uint32 *>().fail("Not enough free memory");
        auto r = reinterpret_cast<uint32 *>(mem::HEAP_START + BS * next_free);
        next_free = find_next_free().value;
        return r;
    }
    constexpr optional<bool> deallocate(void *p) {
        auto target = reinterpret_cast<uint32>(p) - mem::HEAP_START;
        if (!block_list[target]) {
            return optional<bool>().fail(
                "Deallocating empty field: this is a bug");
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
        array<char, 10> str;
        term::write("Cannot allocate more than ",
                    int_to_string(4 * sizeof(uint32), str), " bytes at once\n");
        asm("hlt");
    }
    auto r = fast_allocator.allocate(1);
    if (!r) {
        term::write("Allocation failed: ", r.err, "\n");
        asm("hlt");
    }
    return r.value;
}

void operator delete(void *p) noexcept { fast_allocator.deallocate(p); }
