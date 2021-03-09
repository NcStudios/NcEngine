#include "ColliderTree.h"
#include "Ecs.h"

namespace
{
    constexpr auto initialExtents = 500.0f;
    constexpr size_t BranchDegree = 8u;
    constexpr size_t OctantDensityThreshold = 20u;
    constexpr size_t LeafNodeIndex = 0u;
    constexpr size_t InnerNodeIndex = 1u;
}

namespace nc::physics
{
    Octant::Octant(DirectX::XMFLOAT3 center, float halfSideLength)
        : m_partitionBoundingVolume{center, {halfSideLength, halfSideLength, halfSideLength}},
          m_data{LeafNodeDataType{}} // don't default insert!!
    {
        std::get<LeafNodeIndex>(m_data).reserve(OctantDensityThreshold);
    }

    void Octant::Add(const StaticTreeEntry* newEntry)
    {
        if(!Contains(newEntry->volume))
            return;

        if(auto* staticColliders = std::get_if<LeafNodeIndex>(&m_data); staticColliders)
        {
            if(staticColliders->size() < OctantDensityThreshold)
            {
                staticColliders->push_back(newEntry);
                return;
            }

            Subdivide();
        }

        AddToChildren(newEntry);
    }

    void Octant::Subdivide()
    {
        // calculate points for children
        const auto newExtent = m_partitionBoundingVolume.Extents.x / 2.0f;
        const auto& [centerX, centerY, centerZ] = m_partitionBoundingVolume.Center;
        const auto xMin = centerX - newExtent;
        const auto xMax = centerX + newExtent;
        const auto yMin = centerY - newExtent;
        const auto yMax = centerY + newExtent;
        const auto zMin = centerZ - newExtent;
        const auto zMax = centerZ + newExtent;

        // pull colliders out of variant
        auto containedColliders = std::move(std::get<LeafNodeIndex>(m_data));
        // is vec actually moved?

        // replace collider data with children octants
        m_data.emplace<InnerNodeIndex>(InnerNodeDataType
        {
            Octant{DirectX::XMFLOAT3{xMin, yMin, zMin}, newExtent},
            Octant{DirectX::XMFLOAT3{xMin, yMin, zMax}, newExtent},
            Octant{DirectX::XMFLOAT3{xMin, yMax, zMin}, newExtent},
            Octant{DirectX::XMFLOAT3{xMin, yMax, zMax}, newExtent},
            Octant{DirectX::XMFLOAT3{xMax, yMin, zMin}, newExtent},
            Octant{DirectX::XMFLOAT3{xMax, yMin, zMax}, newExtent},
            Octant{DirectX::XMFLOAT3{xMax, yMax, zMin}, newExtent},
            Octant{DirectX::XMFLOAT3{xMax, yMax, zMax}, newExtent}
        });

        // pass colliders onto children
        for(const auto* collider : containedColliders)
            AddToChildren(collider);
    }

    void Octant::Clear()
    {
        auto& entries = m_data.emplace<LeafNodeIndex>(LeafNodeDataType{});
        entries.reserve(OctantDensityThreshold);
    }

    void Octant::BroadCheck(const DirectX::BoundingSphere& dynamicEstimate, std::vector<const StaticTreeEntry*>* out) const
    {
        if(!Contains(dynamicEstimate))
            return;

        if(const auto* children = std::get_if<InnerNodeIndex>(&m_data); children)
        {
            for(const auto& child : *children)
                child.BroadCheck(dynamicEstimate, out);

            return;
        }

        for(const auto* entry : std::get<LeafNodeIndex>(m_data))
        {
            if(std::visit([&dynamicEstimate](auto&& staticVolume) { return dynamicEstimate.Intersects(staticVolume); }, entry->volume))
                    out->emplace_back(entry);
        }
    }

    void Octant::AddToChildren(const StaticTreeEntry* colliderData)
    {
        for(auto& childSpace : std::get<InnerNodeIndex>(m_data))
            childSpace.Add(colliderData);
    }

    bool Octant::Contains(const Collider::BoundingVolume& other) const
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