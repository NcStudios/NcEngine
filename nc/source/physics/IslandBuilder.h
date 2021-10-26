#pragma once

#include "collision/CollisionCache.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <span>

namespace nc::physics
{
    struct IslandElement
    {
        /** Index of the element's parent. This is self-referential for root
         *  nodes. After sorting, this will point directly to the root. */
        uint32_t id;

        /** The collider's position in the registry. */
        uint32_t colliderIndex;
    };

    /** @todo Look into what it would take to return vector<span> over the input data. */

    /** A union find implementation. By treating a broad phase event as a graph edge, we can
     *  view colliders as disjoint sets, each corresponding to a physically separate simulation
     *  island. */
    class IslandBuilder
    {
        public:
            auto FindIslands(uint32_t totalColliderCount, std::span<const BroadEvent> broadPhysicsEvents) -> std::vector<std::span<IslandElement>>;

        private:
            std::vector<IslandElement> m_elements;
            size_t m_islandCountHint = 0u;

            void Initialize(uint32_t colliderCount);
            void AddPair(uint32_t indexA, uint32_t indexB);
            auto FindRoot(uint32_t a) -> uint32_t;
            void Sort();
            auto BuildIslands() -> std::vector<std::span<IslandElement>>;
    };

    inline auto IslandBuilder::FindIslands(uint32_t totalColliderCount, std::span<const BroadEvent> broadPhysicsEvents) -> std::vector<std::span<IslandElement>>
    {
        Initialize(totalColliderCount);

        for(const auto& event : broadPhysicsEvents)
        {
            AddPair(event.first, event.second);
        }

        Sort();

        return BuildIslands();
    }

    inline void IslandBuilder::Initialize(uint32_t colliderCount)
    {
        /** Initialize to N disjoint sets. */
        m_elements.resize(colliderCount);
        for(auto i = 0u; i < colliderCount; ++i)
        {
            m_elements[i] = IslandElement{i, i};
        }
    }

    inline void IslandBuilder::AddPair(uint32_t indexA, uint32_t indexB)
    {
        /** Merge the sets a and b belong to */
        auto islandA = FindRoot(indexA);
        auto islandB = FindRoot(indexB);

        if(islandA == islandB)
            return;

        m_elements[islandA].id = islandB;
    }

    inline auto IslandBuilder::FindRoot(uint32_t index) -> uint32_t
    {
        assert(index <= m_elements.size());

        /** Traverse the index's set until we find the root,
         *  compressing the path along the way. */
        while(index != m_elements[index].id)
        {
            const auto id = m_elements[m_elements[index].id].id;
            m_elements[index].id = id;
            index = id;
        }

        return index;
    }

    inline void IslandBuilder::Sort()
    {
        uint32_t elementCount = m_elements.size();

        /** Enforce each node points directly to its root. */
        for(uint32_t i = 0u; i < elementCount; ++i)
        {
            m_elements[i].id = FindRoot(i);
        }

        /** Sort by set */
        std::sort(m_elements.begin(), m_elements.end(), [](const auto& a, const auto& b) { return a.id < b.id; });
    }

    auto IslandBuilder::BuildIslands() -> std::vector<std::span<IslandElement>>
    {
        /** Build views over each set.
         *  @todo With lots of small sets, we will end up islands that are too small be
         *  processed as a distinct task. Maybe we should combine those here? */

        std::vector<std::span<IslandElement>> islands;
        islands.reserve(m_islandCountHint);
        size_t elementCount = m_elements.size();
        if(elementCount == 0u)
            return islands;

        size_t id = m_elements[0].id;
        size_t islandStartIndex = 0u;
        size_t index = 1u;

        while(index < elementCount)
        {
            const auto& element = m_elements[index];
            if(id != element.id)
            {
                id = element.id;
                auto beg = m_elements.begin() + islandStartIndex;
                islands.push_back(std::span<IslandElement>(beg, index - islandStartIndex));
                islandStartIndex = index;
            }

            ++index;
        }

        auto beg = m_elements.begin() + islandStartIndex;
        islands.push_back(std::span<IslandElement>(beg, index - islandStartIndex));
        m_islandCountHint = islands.size();
        return islands;
    }
}