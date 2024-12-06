#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/utility/SparseMap.h"

#include <span>
#include <vector>

namespace nc::graphics
{
// Identifier for a range of bones in the BoneCache.
using BoneCacheHandle = uint32_t;

// BoneCache interface for allocating and freeing bone ranges. Events originating from game logic/API should only be
// routed here to avoid race conditions with the actual buffer.
class BoneCacheStaging
{
    struct FreeSlot
    {
        uint32_t index;
        uint32_t capacity;
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

        // Functions for Testing
        auto GetCapacity(BoneCacheHandle handle) { return m_allocations.at(handle); }
        auto GetFreeList() const -> const std::vector<FreeSlot>& { return m_freeList; }

    private:
        sparse_map<uint32_t> m_allocations; // offset -> capacity
        std::vector<FreeSlot> m_freeList;
        uint32_t m_nextIndex = 0;
        uint32_t m_maxIndex;
};

// CPU-side buffer for animated bone matrices. A region is allocated from the buffer per SkinnedMesh to store each of
// its bones contiguously. Allocation returns a handle, indicating an offset where that mesh's bones may be written.
//
// Game logic and skeletal animation calculations may use this buffer concurrently. Allocations must be committed via
// CommitPendingChanges() at a synchronization point when there is no other access to the buffer.
class BoneCache
{
    public:
        explicit BoneCache(uint32_t maxBones)
            : m_staging{maxBones}
        {
        }

        // Get an interface for allocating/freeing ranges within the buffer.
        // IMPORTANT: All API-level events shall only access the staging area.
        auto GetStagingArea() -> BoneCacheStaging& { return m_staging; }

        // Get the total buffer size.
        auto GetCapacity() const -> size_t { return m_data.size(); }

        // Write bone data to the region owned by the handle.
        void UpdateRegion(BoneCacheHandle handle, std::span<const BoneData> data);

        // Commit allocations to the buffer.
        // IMPORTANT: Neither the staging area nor buffer may be accessed while this in progress.
        void CommitPendingChanges();

        // Build info specifying modified buffer ranges
        auto BuildUpdateInfo() -> BufferUpdateInfo<BoneData>;

    private:
        std::vector<BoneData> m_data;
        BoneCacheStaging m_staging;
};
} // namespace nc::graphics
