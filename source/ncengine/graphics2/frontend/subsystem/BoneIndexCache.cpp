#include "BoneIndexCache.h"

#include <cstring>
#include <ranges>

namespace nc::graphics
{
auto BoneIndexCache::Allocate(uint32_t count) -> BoneCacheHandle
{
    const auto slot = std::ranges::find_if(
        m_freeList,
        [count](const auto& freeSlot) {
            return count <= freeSlot.capacity;
        }
    );

    if (slot != m_freeList.end())
    {
        const auto handle = slot->index;
        if (count < slot->capacity)
        {
            slot->index += count;
            slot->capacity -= count;
        }
        else
        {
            *slot = m_freeList.back();
            m_freeList.pop_back();
        }

        m_allocations.emplace(handle, count);
        return handle;
    }

    NC_ASSERT(m_nextIndex + count < m_maxIndex, "BoneCache capacity exceeded");
    const auto handle = m_nextIndex;
    m_nextIndex += count;
    m_allocations.emplace(handle, count);
    return handle;
}

void BoneIndexCache::Free(BoneCacheHandle handle)
{
    const auto capacity = m_allocations.at(handle);
    m_allocations.erase(handle);
    const auto pos = std::ranges::lower_bound(
        m_freeList,
        handle,
        std::ranges::less{},
        &FreeSlot::index
    );

    // if a contigous range can be made with preceeding slot, just add our capacity to that slot
    if (pos != m_freeList.begin())
    {
        const auto left = pos - 1;
        const auto leftEnd = left->index + left->capacity;
        if (leftEnd == handle)
        {
            left->capacity += capacity;
            if (pos != m_freeList.end())
            {
                // if new capacity makes contigous range with the next slot, combine them
                if (left->index + left->capacity == pos->index)
                {
                    left->capacity += pos->capacity;
                    m_freeList.erase(pos);
                }
            }

            return;
        }
    }

    // same check, but on the following slot
    if (pos != m_freeList.end())
    {
        const auto freedEnd = handle + capacity;
        if (pos->index == freedEnd)
        {
            pos->index = handle;
            pos->capacity += capacity;
            return;
        }
    }

    // can't be combined with an existing slot so make a new one
    m_freeList.emplace(pos, handle, capacity);
}
} // namespace nc::graphics
