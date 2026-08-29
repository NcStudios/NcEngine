#include "BoneCache.h"

#include "ncengine/debug/Profile.h"
#include "ncutility/NcError.h"

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

void BoneCacheStaging::Purge()
{
    auto maxIndex = 0u;
    for (const auto& [offset, capacity] : std::views::zip(m_allocations.keys(), m_allocations.values()))
    {
        maxIndex = std::max(maxIndex, offset + capacity);
    }

    m_nextIndex = maxIndex;

    for (const auto& [offset, capacity] : std::exchange(m_freeList, {}))
    {
        if (offset >= maxIndex)
        {
            continue;
        }
        else if (offset + capacity >= maxIndex)
        {
            m_freeList.emplace_back(offset, maxIndex);
        }
        else
        {
            m_freeList.emplace_back(offset, capacity);
        }
    }
}

void BoneCache::CommitPendingChanges()
{
    NC_PROFILE_SCOPE("BoneCache::CommitPendingChanges", ProfileCategory::Animation);
    std::ranges::fill(m_data, BoneData{});
    std::ranges::fill(m_boneNames, std::string{});
    const auto newCapacity = m_staging.GetCapacity();
    if (newCapacity > m_data.size())
    {
        m_data.resize(newCapacity);
        m_boneNames.resize(newCapacity);
    }
}

void BoneCache::UpdateRegion(BoneCacheHandle boneIndex, std::span<const BoneData> bones, std::span<const std::string> boneNames)
{
    NC_PROFILE_SCOPE("BoneCache::UpdateRegion", ProfileCategory::Animation);
    NC_ASSERT(bones.size() == boneNames.size(), "Bone size and bone names mismatch");
    NC_ASSERT(m_data.size() >= bones.size() + boneIndex, "BoneCache write out of bounds");
    NC_ASSERT(m_boneNames.size() >= boneNames.size() + boneIndex, "BoneCache write out of bounds");
    std::memcpy(m_data.data() + boneIndex, bones.data(), bones.size() * sizeof(BoneData));
    std::ranges::copy(boneNames, m_boneNames.begin() + boneIndex);
}

auto BoneCache::BuildUpdateInfo() -> BufferUpdateInfo<BoneData>
{
    const auto size = static_cast<uint32_t>(m_data.size());
    return size != 0u
        ? BufferUpdateInfo<BoneData>{m_data, { {0, size} }}
        : BufferUpdateInfo<BoneData>{};
}

void BoneCache::Purge()
{
    NC_PROFILE_SCOPE("BoneCache::Purge", ProfileCategory::Animation);
    m_staging.Purge();
    m_data.resize(m_staging.GetCapacity());
    m_data.shrink_to_fit();
    m_boneNames.resize(m_staging.GetCapacity());
    m_boneNames.shrink_to_fit();
}

auto BoneCache::GetBoneData(uint64_t meshId, const std::string& boneName) const -> BoneData
{
    auto boneId = std::to_string(meshId) + boneName;
    auto pos = std::ranges::find(m_boneNames, boneId);
    if (pos == m_boneNames.end())
    {
        throw nc::NcError("Bone does not exist in dataset.");
    }

    auto index = static_cast<uint32_t>(std::distance(m_boneNames.begin(), pos));
    return m_data.at(index);
}

} // namespace nc::graphics
