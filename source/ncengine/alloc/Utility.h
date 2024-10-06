#pragma once

#include "Allocator.h"

namespace nc::alloc
{
    template<class Alloc>
    struct basic_deleter
    {
        void operator()(Alloc::value_type* ptr)
        {
            Alloc().deallocate(ptr, 1);
        }
    };

    /** Unique pointer helper for custom allocators. */
    template<class T, class Alloc, class... Args>
    auto make_unique(Args&&... args)
    {
        using deleter = basic_deleter<Alloc>;
        auto* ptr = Alloc().allocate(1);
        ptr = new(ptr) T{std::forward<Args>(args)...};
        return std::unique_ptr<T, deleter>(ptr, deleter{});
    }
} // namespace nc::alloc