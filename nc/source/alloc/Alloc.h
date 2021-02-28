#pragma once

#include "Allocator.h"

namespace nc::alloc
{
    /** Convenience type for single-type pool allocator. Tag parameter can be
     *  used to differentiate pools of the same type. */
    template<class T, class Tag = void>
    using PoolAllocator = FixedTypeAllocator<StaticPool<T, Tag>>;

    /** @note This guarantees empty base class optimization. Stateful allocators
     *  would require a different deleter. */
    template<class Alloc>
        requires std::is_empty_v<Alloc>
    struct basic_deleter : private Alloc
    {
        void operator()(Alloc::value_type* ptr)
        {
            Alloc::deallocate(ptr, 1);
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