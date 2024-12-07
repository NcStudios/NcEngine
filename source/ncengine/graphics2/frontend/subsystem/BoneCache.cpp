#include "BoneCache.h"

#include <cstring>
#include <ranges>

namespace nc::graphics
{
auto BoneCacheStaging::Allocate(uint32_t count) -> BoneCacheHandle
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

void BoneCacheStaging::Free(BoneCacheHandle handle)
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

void BoneCache::UpdateRegion(BoneCacheHandle handle, std::span<const BoneData> data)
{
    NC_ASSERT(m_data.size() >= handle + data.size(), "BoneCache access out of bounds");
    std::memcpy(m_data.data() + handle, data.data(), sizeof(BoneData) * data.size());
}

void BoneCache::CommitPendingChanges()
{
    const auto capacity = m_staging.GetCapacity();
    if (capacity > m_data.size())
    {
        m_data.resize(capacity);
    }
}

auto BoneCache::BuildUpdateInfo() -> BufferUpdateInfo<BoneData>
{
    return m_data.empty()
        ? BufferUpdateInfo<BoneData>{}
        : BufferUpdateInfo<BoneData>{
            .instances = m_data,
            .dirtyRanges = { {0, static_cast<uint32_t>(m_data.size())} }
        };
}
} // namespace nc::graphics
