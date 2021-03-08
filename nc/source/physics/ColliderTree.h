#pragma once

#include "CollisionUtility.h"
#include "alloc/Alloc.h"

#include <memory>
#include <vector>
#include <variant>

namespace nc::physics
{
    struct StaticTreeEntry;

    /** ColliderTree node representing a cubic chunk of the world, subdividing into
     *  child partitions when the number of contained colliders reaches a threshold. */
    class WorldspacePartition
    {

        /** can get rid of isLeaf? */

        public:
            WorldspacePartition(DirectX::XMFLOAT3 center, float halfSideLength);

            void Add(const StaticTreeEntry* colliderData);
            void Branch();
            void Clear();
            void BroadCheck(const DirectX::BoundingSphere& dynamicVolume, std::vector<const StaticTreeEntry*>* out) const;

        private:

            void AddToChildren(const StaticTreeEntry* colliderData);
            bool Contains(const Collider::BoundingVolume& other) const;

            DirectX::BoundingBox m_partitionBoundingVolume;
            std::variant<std::vector<WorldspacePartition>, std::vector<const StaticTreeEntry*>> m_data;
            //std::vector<const StaticTreeEntry*> m_collidersInPartition;
            //std::vector<WorldspacePartition> m_subspaces;
            //bool isLeaf;
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
            WorldspacePartition m_root;
    };
} // namespace nc::physics