#pragma once

#include "Allocator.h"
#include "Pool.h"

namespace nc::alloc
{
    template<class T, class Tag = void>
    using PoolAllocator = Allocator<Pool<T, Tag>>;

    template<class T, class Alloc, class... Args>
    auto make_unique(Args&&... args)
    {
        using deleter = Alloc::deleter;
        auto* ptr = Alloc().allocate(1);
        ptr = new(ptr) T{std::forward<Args>(args)...};
        return std::unique_ptr<T, deleter>(ptr, deleter{});
    }
} // namespace nc::alloc