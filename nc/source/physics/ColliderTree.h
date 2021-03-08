#pragma once

#include "CollisionUtility.h"
#include "alloc/Alloc.h"

#include <memory>
#include <vector>
#include <variant>

namespace nc::physics
{
    struct StaticTreeEntry;

    /** ColliderTree node representing a cubic chunk of the world. Leaf nodes contain
     *  pointers to collider data for every static collider contained in the octants
     *  bounding volume, while inner nodes contain a list of children octants. */
    class Octant
    {
        // is it worth storing static estimates too?

        public:
            Octant(DirectX::XMFLOAT3 center, float halfSideLength);
            void Add(const StaticTreeEntry* newEntry);
            void Branch();
            void Clear();
            void BroadCheck(const DirectX::BoundingSphere& dynamicEstimate, std::vector<const StaticTreeEntry*>* out) const;

        private:
            void AddToChildren(const StaticTreeEntry* colliderData);
            bool Contains(const Collider::BoundingVolume& other) const;

            DirectX::BoundingBox m_partitionBoundingVolume;
            std::variant<std::vector<const StaticTreeEntry*>, std::vector<Octant>> m_data;
    };

    /** A tree which collects static colliders into nodes based on their spatial locality. */
    class ColliderTree
    {
        public:
            ColliderTree();
            /** not really noexcept */
            ~ColliderTree() noexcept;

            void Add(EntityHandle handle, const ColliderInfo& info);
            void Remove(EntityHandle handle);
            void Clear();
            std::vector<const StaticTreeEntry*> BroadCheck(const DirectX::BoundingSphere& volume) const;

        private:
            using Allocator = alloc::PoolAllocator<StaticTreeEntry>;
            std::vector<std::unique_ptr<StaticTreeEntry, alloc::basic_deleter<Allocator>>> m_staticEntries;
            Octant m_root;
    };
} // namespace nc::physics