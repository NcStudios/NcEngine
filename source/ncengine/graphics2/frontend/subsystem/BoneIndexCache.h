#pragma once

#include "ncengine/utility/SparseMap.h"

#include <vector>

namespace nc::graphics
{
// Identifier for a range of bones in the BoneCache.
using BoneCacheHandle = uint32_t;

// BoneCache interface for allocating and freeing bone ranges. Events originating from game logic/API should only be
// routed here to avoid race conditions with the actual buffer.
class BoneIndexCache
{
    struct FreeSlot
    {
        uint32_t index;
        uint32_t capacity;
    };

    public:
        explicit BoneIndexCache(uint32_t maxBones)
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
} // namespace nc::graphics
