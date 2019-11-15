#include "core.hpp"
#include "multiboot.h"
#include "term.hpp"

namespace mod {

class module_list {
    using module_proc = int (*)(void);
    static constexpr uint32 ELF_OFFSET = 0x1000;
    multiboot_module_t *addr_;
    int count_;

  public:
    module_list(multiboot_info_t const &mb)
        : addr_(reinterpret_cast<multiboot_module_t *>(mb.mods_addr)),
          count_(static_cast<int>(mb.mods_count)) {}

    optional<module_proc> entry_point(int mod) {
        if (mod < count_)
            return reinterpret_cast<module_proc>(addr_[mod].mod_start +
                                                 ELF_OFFSET);
        else
            return {};
    }

    auto size() const { return count_; }
    constexpr auto begin() const {
        return dual_iterator<multiboot_module_t>(addr_);
    }
    constexpr auto end() const {
        return dual_iterator<multiboot_module_t>(addr_ + size());
    }
};

} // namespace mod
