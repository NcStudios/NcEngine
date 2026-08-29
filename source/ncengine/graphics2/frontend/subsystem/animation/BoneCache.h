#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/utility/SparseMap.h"

#include <limits>

namespace nc::graphics
{
// Identifier for a range of bones in the BoneCache.
using BoneCacheHandle = uint32_t;

// Invalid or empty bone cache allocation.
constexpr auto NullBoneCacheHandle = std::numeric_limits<uint32_t>::max();

// BoneCache interface for allocating and freeing bone ranges. Events originating from game logic/API should only be
// routed here to avoid race conditions with the actual buffer.
class BoneCacheStaging
{
    struct FreeSlot
    {
        uint32_t index = 0u;
        uint32_t capacity = 0u;
    };

    public:
        explicit BoneCacheStaging(uint32_t maxBones)
            : m_allocations{0u, maxBones},
              m_maxIndex{maxBones}
        {
        }

        /// API-Facing Functions
        auto Allocate(uint32_t count) -> BoneCacheHandle;
        void Free(BoneCacheHandle handle);

        // Functions for Use by BoneCache
        auto GetCapacity() const -> size_t { return m_nextIndex; }
        void Purge();

        // Functions for Testing
        auto GetCapacity(BoneCacheHandle handle) { return m_allocations.at(handle); }
        auto GetFreeList() const -> const std::vector<FreeSlot>& { return m_freeList; }

    private:
        sparse_map<uint32_t> m_allocations; // offset -> capacity
        std::vector<FreeSlot> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;
};

// CPU-side buffer for animated bone matrices. Has very similar behavior to StructuredHostBuffer, but the buffer is
// completely overwritten each frame by the SkeletalAnimationSubsystem. Because of this, the staging area just hands
// out indices without needing to write anything.
class BoneCache
{
    public:
        explicit BoneCache(uint32_t maxBones)
            : m_staging{maxBones}
        {
        }

        // Get the staging area for allocating bones.
        auto GetStagingArea() -> BoneCacheStaging& { return m_staging; }

        // Get total buffer size
        auto GetCapacity() const -> size_t { return m_data.size(); }

        // Zero the buffer and reallocate based on staged allocations.
        void CommitPendingChanges();

        // Update a range of bones at the offset owned by boneIndex.
        void UpdateRegion(BoneCacheHandle boneIndex, std::span<const BoneData> bones, std::span<const std::string> boneNames);

        // Build info specifying modified buffer ranges.
        auto BuildUpdateInfo() -> BufferUpdateInfo<BoneData>;

        // Clear the buffer and compress the staging area.
        void Purge();

        // Get a copy of the bone data at a given index
        auto GetBoneData(uint64_t meshId, const std::string& boneName) const -> BoneData;

    private:
        std::vector<std::string> m_boneNames;
        std::vector<BoneData> m_data;
        BoneCacheStaging m_staging;
};
} // namespace nc::graphics
