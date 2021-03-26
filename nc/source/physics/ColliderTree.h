#pragma once

#include "CollisionUtility.h"
#include "alloc/Utility.h"

#include <memory>
#include <vector>
#include <variant>

namespace nc::physics
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

    /** ColliderTree node representing a cubic chunk of the world. Leaf nodes store
     *  data for each static collider contained within its bounding volume, while
     *  inner nodes contain a list of children octants. */
    class Octant
    {
        public:
            Octant(DirectX::XMFLOAT3 center, float halfSideLength);
            void Add(const StaticTreeEntry* newEntry);
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

    /** @todo worldspaceExtent is just a constant set in PhysicsSystem. Eventually,
     *  this should be read from somewhere. Maybe scenes? If scenes can specify this,
     *  we will also need a way to resize extents of the root upon changing scene. */
    
    /** @todo With omega-dense static geometry, octants can become smaller than the average
     *  mesh size. From what I can tell, static geometry computations are trivial until
     *  this happens. If we could specify some minimum extent, and use the density threshold
     *  as a preference rather than requirement, this would stop happening. Generally, scenes
     *  won't have thousands of static things in a tiny area, but maybe it could happen? */

    /** An octree for static colliders. */
    class ColliderTree
    {
        public:
            ColliderTree(uint32_t maxStaticColliders, uint32_t densityThreshold, float worldspaceExtent);
            ~ColliderTree() noexcept;

            void Add(EntityHandle handle, const ColliderInfo& info);
            void Remove(EntityHandle handle);
            void Rebuild();
            void Clear();
            std::vector<const StaticTreeEntry*> BroadCheck(const DirectX::BoundingSphere& volume) const;

        private:
            using Allocator = alloc::PoolAllocator<StaticTreeEntry>;
            std::vector<std::unique_ptr<StaticTreeEntry, alloc::basic_deleter<Allocator>>> m_staticEntries;
            Octant m_root;
    };
} // namespace nc::physics