#pragma once

#include "collision/CollisionCache.h"

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
}