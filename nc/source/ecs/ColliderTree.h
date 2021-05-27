#pragma once

#include "alloc/PoolAdapter.h"
#include "physics/CollisionUtility.h"

#include <vector>
#include <variant>

namespace nc
{
    namespace ecs { struct StaticTreeEntry; }

    template<>
    struct StoragePolicy<ecs::StaticTreeEntry>
    {
        using allow_trivial_destruction = std::true_type;
        using sort_dense_storage_by_address = std::true_type;
        // ignore callbacks
    };
}

namespace nc::ecs
{
    /** Here's a thought: can dynamic colliders be in a tree too if we add them using
     *  their estimates? It would have to be built every frame, but we're already doing
     *  a minimum of n choose 2 sphere comparisons per frame.
     *  
     *  Could end up with:
     *  for each collider [n]
     *      compare to octants until we find a leaf [log(avgTreeHeight)]
     *  for each leaf in tree [n / DensityThreshold]
     *      compare each unordered pair [DensityThreshold choose 2]
     *  or: n*log(height) + (n/thresh)*thresh*thresh = n*log(height) + n*thresh^2 = n*thresh^2
     * 
     *  if we make thresh = log(n) [or maybe log(maxColliders) for easy peasy-ness]
     *  = n * log(n) * log(n) = n * log(n)
     **/

    /** @todo This may be faster if the vectors in here came from an allocator. Also,
     *  they don't have to be vectors. Array is slower because it makes the nodes too large,
     *  but maybe ptr to array from allocator? */

    /** The bounding volume and handle of a static collider in the octree. */
    struct StaticTreeEntry
    {
        Collider::BoundingVolume volume;
        physics::Layer layer;
        EntityHandle handle;
    };

    /** ColliderTree node representing a cubic chunk of the world. Leaf nodes store
     *  data for each static collider contained within its bounding volume, while
     *  inner nodes contain a list of children octants. */
    class Octant
    {
        public:
            Octant(DirectX::XMFLOAT3 center, float halfSideLength);
            void Add(const StaticTreeEntry* newEntry);
            bool AtMinimumExtent() const;
            void Subdivide();
            void Clear();
            void BroadCheck(const DirectX::BoundingSphere& dynamicEstimate, std::vector<const StaticTreeEntry*>* out) const;
            float GetExtent() const noexcept;

        private:
            void AddToChildren(const StaticTreeEntry* colliderData);
            void GetEntriesFromChildren(std::vector<const StaticTreeEntry*>* out) const;
            bool Contains(const Collider::BoundingVolume& other) const;

            using LeafNodeDataType = std::vector<const StaticTreeEntry*>;
            using InnerNodeDataType = std::vector<Octant>;

            DirectX::BoundingBox m_boundingVolume;
            std::variant<LeafNodeDataType, InnerNodeDataType> m_data;
    };

    /** possible @todo Tree could toggle between quad/oct depending on config. Some
     *  games may not benefit from subdividing across 3 dimensions */

    /** An octree for static colliders. */
    class ColliderTree
    {
        public:
            ColliderTree(uint32_t maxStaticColliders, uint32_t densityThreshold, float minimumExtent, float worldspaceExtent);

            void Add(EntityHandle handle, const ColliderInfo& info);
            void Remove(EntityHandle handle);
            void Rebuild();
            void Clear();
            std::vector<const StaticTreeEntry*> BroadCheck(const DirectX::BoundingSphere& volume) const;

        private:
            alloc::PoolAdapter<StaticTreeEntry> m_pool;
            Octant m_root;
    };
} // namespace nc::ecs