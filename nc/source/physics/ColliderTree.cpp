#include "ColliderTree.h"
#include "Ecs.h"

namespace
{
    constexpr auto initialExtents = 500.0f;
    constexpr uint32_t SubspaceBranchCount = 8u;
    constexpr uint32_t MaxVolumesPerPartition = 20u;

    constexpr size_t WorldspacePartitionIndex = 0u;
    constexpr size_t StaticTreeEntryIndex = 1u;
}

namespace nc::physics
{
    WorldspacePartition::WorldspacePartition(DirectX::XMFLOAT3 center, float halfSideLength)
        : m_partitionBoundingVolume{center, {halfSideLength, halfSideLength, halfSideLength}},
          m_data{std::vector<const StaticTreeEntry*>{}}
          //m_collidersInPartition{},
          //m_subspaces{},
          //isLeaf{true}
    {
        std::get<StaticTreeEntryIndex>(m_data).reserve(MaxVolumesPerPartition);
        //m_collidersInPartition.reserve(MaxVolumesPerPartition);
        //m_subspaces.reserve(SubspaceBranchCount);
    }

    void WorldspacePartition::Add(const StaticTreeEntry* colliderData)
    {
        if(!Contains(colliderData->volume))
            return;

        if(m_data.index() == StaticTreeEntryIndex)
        {
            auto& entries = std::get<StaticTreeEntryIndex>(m_data);
            if(entries.size() < MaxVolumesPerPartition)
            {
                entries.push_back(colliderData);
                return;
            }

            Branch();
        }

        AddToChildren(colliderData);

        // if(isLeaf)
        // {
        //     if(m_collidersInPartition.size() < MaxVolumesPerPartition)
        //     {
        //         m_collidersInPartition.push_back(colliderData);
        //         return;
        //     }

        //     Branch();
        // }
        
        // AddToChildren(colliderData);
    }

    void WorldspacePartition::Branch()
    {
        auto entriesCopy = std::get<StaticTreeEntryIndex>(m_data);


        //isLeaf = false;
        const auto newHalfSideLength = m_partitionBoundingVolume.Extents.x / 2.0f;
        const auto& [centerX, centerY, centerZ] = m_partitionBoundingVolume.Center;
        const auto xMin = centerX - newHalfSideLength;
        const auto xMax = centerX + newHalfSideLength;
        const auto yMin = centerY - newHalfSideLength;
        const auto yMax = centerY + newHalfSideLength;
        const auto zMin = centerZ - newHalfSideLength;
        const auto zMax = centerZ + newHalfSideLength;

        m_data = std::vector<WorldspacePartition>{};
        auto& subspaces = std::get<WorldspacePartitionIndex>(m_data);
        subspaces.reserve(8u);

        subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMin, zMin}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMin, zMax}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMax, zMin}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMax, zMax}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMin, zMin}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMin, zMax}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMax, zMin}, newHalfSideLength);
        subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMax, zMax}, newHalfSideLength);

        for(const auto* colliderData : entriesCopy)
            AddToChildren(colliderData);

        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMin, zMin}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMin, zMax}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMax, zMin}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMin, yMax, zMax}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMin, zMin}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMin, zMax}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMax, zMin}, newHalfSideLength);
        // m_subspaces.emplace_back(DirectX::XMFLOAT3{xMax, yMax, zMax}, newHalfSideLength);

        // for(const auto* colliderData : m_collidersInPartition)
        //     AddToChildren(colliderData);

        // m_collidersInPartition.clear();
    }

    void WorldspacePartition::Clear()
    {
        m_data = std::vector<const StaticTreeEntry*>{};
        //m_collidersInPartition.clear();
        //m_subspaces.clear();
        //isLeaf = true;
    }

    void WorldspacePartition::BroadCheck(const DirectX::BoundingSphere& dynamicVolume, std::vector<const StaticTreeEntry*>* out) const
    {
        if(!Contains(dynamicVolume))
            return;

        if(m_data.index() == StaticTreeEntryIndex)
        {
            const auto& entries = std::get<StaticTreeEntryIndex>(m_data);
            for(const auto* colliderData : entries)
            {
                if(std::visit([&dynamicVolume](auto&& staticVolume) { return dynamicVolume.Intersects(staticVolume); }, colliderData->volume))
                    out->emplace_back(colliderData);
            }

            return;
        }

        const auto& subspaces = std::get<WorldspacePartitionIndex>(m_data);
        for(const auto& childSpace : subspaces)
        {
            childSpace.BroadCheck(dynamicVolume, out);
        }

        // if(isLeaf)
        // {
        //     for(const auto* colliderData : m_collidersInPartition)
        //     {
        //         if(std::visit([&dynamicVolume](auto&& staticVolume) { return dynamicVolume.Intersects(staticVolume); }, colliderData->volume))
        //             out->emplace_back(colliderData);
        //     }

        //     return;
        // }

        // for(const auto& childSpace : m_subspaces)
        // {
        //     childSpace.BroadCheck(dynamicVolume, out);
        // }
    }

    void WorldspacePartition::AddToChildren(const StaticTreeEntry* colliderData)
    {
        for(auto& childSpace : std::get<WorldspacePartitionIndex>(m_data))
        {
            childSpace.Add(colliderData);
        }
        // for(auto& childSpace : m_subspaces)
        // {
        //     childSpace.Add(colliderData);
        // }
    }

    bool WorldspacePartition::Contains(const Collider::BoundingVolume& other) const
    {
        return std::visit([this](auto&& a) { return a.Intersects(m_partitionBoundingVolume); }, other);
    }

    ColliderTree::ColliderTree()
        : m_staticEntries{},
          m_root{{0.0f, 0.0f, 0.0f}, initialExtents}
    {
        uint32_t maxColliders = 1000000;
        Allocator().initialize_memory_resource(maxColliders);
    }

    ColliderTree::~ColliderTree()
    {
        Allocator().release_memory_resource();
    }

    void ColliderTree::Add(EntityHandle handle, const ColliderInfo& info)
    {
        auto volume = CalculateBoundingVolume(info.type, GetVolumePropertiesFromColliderInfo(info), &GetComponent<Transform>(handle)->GetTransformationMatrix());
        m_staticEntries.push_back(alloc::make_unique<StaticTreeEntry, Allocator>(volume, handle));
        m_root.Add(m_staticEntries.back().get());
    }

    void ColliderTree::Remove(EntityHandle handle)
    {
        (void)handle;
    }

    void ColliderTree::Clear()
    {
        m_root.Clear();
        Allocator().clear_memory_resource();
    }

    std::vector<const StaticTreeEntry*> ColliderTree::BroadCheck(const DirectX::BoundingSphere& volume) const
    {
        std::vector<const StaticTreeEntry*> out;
        m_root.BroadCheck(volume, &out);
        return out;
    }
}