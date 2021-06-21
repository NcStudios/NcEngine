#include "ColliderTree.h"
#include "Ecs.h"
#include "physics/IntersectionQueries.h"

namespace
{
    uint32_t DensityThreshold = 20u;
    float MinimumExtent = 5.0f;
    constexpr size_t BranchDegree = 8u;
    constexpr size_t LeafNodeIndex = 0u;
    constexpr size_t InnerNodeIndex = 1u;
}

namespace nc::ecs
{
    Octant::Octant(Vector3 center, float extent)
        : m_boundingVolume{center, Vector3::Splat(extent), extent / 2.0f},
          m_data{LeafNodeDataType{}} // don't default insert!!
    {
        std::get<LeafNodeIndex>(m_data).reserve(DensityThreshold);
    }

    void Octant::Add(const StaticTreeEntry* newEntry)
    {
        if(!physics::Gjk(newEntry->volume, m_boundingVolume, newEntry->matrix, DirectX::XMMatrixIdentity()))
            return;

        if(auto* staticColliders = std::get_if<LeafNodeIndex>(&m_data); staticColliders)
        {
            if(staticColliders->size() < DensityThreshold || AtMinimumExtent())
            {
                staticColliders->push_back(newEntry);
                return;
            }

            Subdivide();
        }

        AddToChildren(newEntry);
    }

    bool Octant::AtMinimumExtent() const
    {
        return m_boundingVolume.extents.x / 2.0f < MinimumExtent;
    }

    void Octant::Subdivide()
    {
        // calculate points for children
        const auto newExtent = m_boundingVolume.extents.x / 2.0f;
        const auto& [centerX, centerY, centerZ] = m_boundingVolume.center;
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
            Octant{Vector3{xMin, yMin, zMin}, newExtent},
            Octant{Vector3{xMin, yMin, zMax}, newExtent},
            Octant{Vector3{xMin, yMax, zMin}, newExtent},
            Octant{Vector3{xMin, yMax, zMax}, newExtent},
            Octant{Vector3{xMax, yMin, zMin}, newExtent},
            Octant{Vector3{xMax, yMin, zMax}, newExtent},
            Octant{Vector3{xMax, yMax, zMin}, newExtent},
            Octant{Vector3{xMax, yMax, zMax}, newExtent}
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

    void Octant::BroadCheck(physics::SphereCollider collider, std::vector<const StaticTreeEntry*>* out) const
    {
        if(!physics::Intersect(collider, m_boundingVolume))
            return;

        if(const auto* children = std::get_if<InnerNodeIndex>(&m_data); children)
        {
            for(const auto& child : *children)
                child.BroadCheck(collider, out);
            return;
        }

        for(const auto* entry : std::get<LeafNodeIndex>(m_data))
        {
            if(physics::Intersect(collider, entry->volumeEstimate))
                out->emplace_back(entry);
        }
    }

    float Octant::GetExtent() const noexcept
    {
        return m_boundingVolume.extents.x;
    }

    void Octant::AddToChildren(const StaticTreeEntry* colliderData)
    {
        for(auto& childSpace : std::get<InnerNodeIndex>(m_data))
            childSpace.Add(colliderData);
    }

    ColliderTree::ColliderTree(uint32_t maxStaticColliders, uint32_t densityThreshold, float minimumExtent, float worldspaceExtent)
        : m_pool{maxStaticColliders},
          m_root{Vector3::Zero(), worldspaceExtent}
    {
        DensityThreshold = densityThreshold;
        MinimumExtent = minimumExtent;
    }

    void ColliderTree::Add(Entity entity, const Collider::VolumeInfo& info)
    {
        auto* registry = ActiveRegistry();
        auto matrix = registry->Get<Transform>(entity)->GetTransformationMatrix();
        auto bv = physics::CreateBoundingVolume(info);
        auto estimate = physics::EstimateBoundingVolume(bv, matrix);
        auto* entry = m_pool.Add(matrix,
                                 bv,
                                 estimate,
                                 physics::ToLayerMask(EntityUtils::Layer(entity)),
                                 entity);

        m_root.Add(entry);
    }

    void ColliderTree::Remove(Entity entity)
    {
        if(m_pool.RemoveIf([entity](auto* e) { return e->entity == entity; }))
            Rebuild();
    }

    void ColliderTree::Rebuild()
    {
        m_root.Clear();
        m_root = Octant{{}, m_root.GetExtent()};

        for(auto* entry : m_pool.GetActiveRange())
            m_root.Add(entry);
    }

    void ColliderTree::Clear()
    {
        m_root.Clear();
        m_pool.Clear();
    }

    std::vector<const StaticTreeEntry*> ColliderTree::BroadCheck(const physics::SphereCollider& volume) const
    {
        std::vector<const StaticTreeEntry*> out;
        m_root.BroadCheck(volume, &out);
        return out;
    }
} // namespace nc::ecs