#include "ColliderTree.h"
#include "Ecs.h"

namespace
{
    constexpr size_t BranchDegree = 8u;
    constexpr size_t DensityThreshold = 20u;
    constexpr size_t LeafNodeIndex = 0u;
    constexpr size_t InnerNodeIndex = 1u;
}

namespace nc::physics
{
    Octant::Octant(DirectX::XMFLOAT3 center, float extent)
        : m_boundingVolume{center, {extent, extent, extent}},
          m_data{LeafNodeDataType{}} // don't default insert!!
    {
        std::get<LeafNodeIndex>(m_data).reserve(DensityThreshold);
    }

    void Octant::Add(const StaticTreeEntry* newEntry)
    {
        if(!Contains(newEntry->volume))
            return;

        if(auto* staticColliders = std::get_if<LeafNodeIndex>(&m_data); staticColliders)
        {
            if(staticColliders->size() < DensityThreshold)
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
        const auto newExtent = m_boundingVolume.Extents.x / 2.0f;
        const auto& [centerX, centerY, centerZ] = m_boundingVolume.Center;
        const auto xMin = centerX - newExtent;
        const auto xMax = centerX + newExtent;
        const auto yMin = centerY - newExtent;
        const auto yMax = centerY + newExtent;
        const auto zMin = centerZ - newExtent;
        const auto zMax = centerZ + newExtent;

        // copy contained colliders
        auto containedColliders = std::move(std::get<LeafNodeIndex>(m_data));

        // replace colliders with children octants
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
        entries.reserve(DensityThreshold);
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

    float Octant::GetExtent() const noexcept
    {
        return m_boundingVolume.Extents.x;
    }

    void Octant::AddToChildren(const StaticTreeEntry* colliderData)
    {
        for(auto& childSpace : std::get<InnerNodeIndex>(m_data))
            childSpace.Add(colliderData);
    }

    bool Octant::Contains(const Collider::BoundingVolume& other) const
    {
        return std::visit([this](auto&& a) { return a.Intersects(m_boundingVolume); }, other);
    }

    ColliderTree::ColliderTree(uint32_t maxStaticColliders, float worldspaceExtent)
        : m_staticEntries{},
          m_root{{0.0f, 0.0f, 0.0f}, worldspaceExtent}
    {
        Allocator().initialize_memory_resource(maxStaticColliders);
    }

    ColliderTree::~ColliderTree() noexcept
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
        auto pos = std::find_if(m_staticEntries.begin(), m_staticEntries.end(), [handle](auto& entry)
        {
            return handle == entry->handle;
        });

        if(pos == m_staticEntries.end())
            throw std::runtime_error("ColliderTree::Remove - bad handle");
        
        *pos = std::move(m_staticEntries.back());
        m_staticEntries.pop_back();
        Rebuild();
    }

    void ColliderTree::Rebuild()
    {
        m_root.Clear();
        m_root = Octant{{}, m_root.GetExtent()};
        for(const auto& entry : m_staticEntries)
            m_root.Add(entry.get());
    }

    void ColliderTree::Clear()
    {
        m_root.Clear();
        m_staticEntries.clear();
        Allocator().clear_memory_resource();
    }

    std::vector<const StaticTreeEntry*> ColliderTree::BroadCheck(const DirectX::BoundingSphere& volume) const
    {
        std::vector<const StaticTreeEntry*> out;
        m_root.BroadCheck(volume, &out);
        return out;
    }
}