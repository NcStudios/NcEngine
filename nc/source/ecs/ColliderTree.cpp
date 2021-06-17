#include "ColliderTree.h"
#include "Ecs.h"
#include "physics/Gjk.h"

namespace
{
    uint32_t DensityThreshold = 20u;
    float MinimumExtent = 5.0f;
    constexpr size_t BranchDegree = 8u;
    constexpr size_t LeafNodeIndex = 0u;
    constexpr size_t InnerNodeIndex = 1u;
}


#include "ColliderSystem.h"
#include <iostream>

namespace nc::ecs
{
    SphereCollider EstimateBoundingVolume(const ecs::VolumeProperties& volumeProperties, DirectX::FXMMATRIX transform)
    {
        auto xExtent_v = DirectX::XMVector3Dot(transform.r[0], transform.r[0]);
        auto yExtent_v = DirectX::XMVector3Dot(transform.r[1], transform.r[1]);
        auto zExtent_v = DirectX::XMVector3Dot(transform.r[2], transform.r[2]);
        auto maxExtent_v = DirectX::XMVectorMax(xExtent_v, DirectX::XMVectorMax(yExtent_v, zExtent_v));
        float maxTransformExtent = sqrt(DirectX::XMVectorGetX(maxExtent_v));

        auto center_v = DirectX::XMLoadVector3(&volumeProperties.center);
        center_v = DirectX::XMVector3Transform(center_v, transform);
        
        std::cout << "maxTransformsExtent:  " << maxTransformExtent << '\n'
                  << "properties.maxExtent: " << volumeProperties.maxExtent << '\n';

        SphereCollider out{Vector3::Zero(), maxTransformExtent * volumeProperties.maxExtent};
        DirectX::XMStoreVector3(&out.center, center_v);
        return out;
    }

    VolumeProperties GetVolumeProperties(const ColliderInfo& info)
    {
        // for mesh should get support in x/y/z for maxExtent

        float maxExtent = 0.0f;
        switch(info.type)
        {
            case ColliderType::Box:
            {
                maxExtent = Magnitude(info.scale / 2.0f);
                //maxExtent = Magnitude(info.scale) / 2.0f;
                break;
            }
            case ColliderType::Sphere:
            {
                maxExtent = info.scale.x / 2.0f;
                break;
            }
            case ColliderType::Mesh: // fix
            {
                maxExtent = 0.5f;
                break;
            }
        }

        return VolumeProperties
        {
            info.offset,
            info.scale / 2.0f,
            maxExtent
        };
    }

    ///////////////////////////////////


    Octant::Octant(Vector3 center, float extent)
        : m_boundingVolume{center, Vector3::Splat(extent)},
          m_data{LeafNodeDataType{}} // don't default insert!!
    {
        std::get<LeafNodeIndex>(m_data).reserve(DensityThreshold);
    }

    void Octant::Add(const StaticTreeEntry* newEntry)
    {
        if(!physics::GJK(newEntry->volume, m_boundingVolume, newEntry->matrix, DirectX::XMMatrixIdentity()))
            return;
        //if(!Contains(newEntry->volume))
        //    return;

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

    void Octant::BroadCheck(SphereCollider collider, std::vector<const StaticTreeEntry*>* out) const
    {
        if(!physics::GJK(collider, m_boundingVolume))
            return;

        if(const auto* children = std::get_if<InnerNodeIndex>(&m_data); children)
        {
            for(const auto& child : *children)
                child.BroadCheck(collider, out);
            return;
        }

        for(const auto* entry : std::get<LeafNodeIndex>(m_data))
        {
            //if(physics::GJK(collider, entry->volume))
            //    out->emplace_back(entry);
            if(physics::GJK(collider, entry->volume, DirectX::XMMatrixIdentity(), entry->matrix))
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

    void ColliderTree::Add(Entity entity, const ColliderInfo& info)
    {
        auto* registry = ActiveRegistry();
        auto matrix = registry->Get<Transform>(entity)->GetTransformationMatrix();
        auto* entry = m_pool.Add(matrix,
                                 CreateBoundingVolume(info.type, info.offset, info.scale),
                                 EstimateBoundingVolume(GetVolumeProperties(info), matrix),
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

    std::vector<const StaticTreeEntry*> ColliderTree::BroadCheck(const SphereCollider& volume) const
    {
        std::vector<const StaticTreeEntry*> out;
        m_root.BroadCheck(volume, &out);
        return out;
    }
} // namespace nc::ecs