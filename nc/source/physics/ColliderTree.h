#pragma once

#include "CollisionUtility.h"
#include "alloc/Alloc.h"

#include <memory>
#include <vector>
#include <variant>

namespace nc::physics
{
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

        private:
            void AddToChildren(const StaticTreeEntry* colliderData);
            bool Contains(const Collider::BoundingVolume& other) const;

            using LeafNodeDataType = std::vector<const StaticTreeEntry*>;
            using InnerNodeDataType = std::vector<Octant>;

            DirectX::BoundingBox m_partitionBoundingVolume;
            std::variant<LeafNodeDataType, InnerNodeDataType> m_data;
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