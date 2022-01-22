#include "MeshBatcher.h"
#include "ecs/Registry.h"
#include "physics/collision/IntersectionQueries.h"

namespace
{
    using namespace nc;

    bool IsViewedByFrustum(const Frustum& frustum, const MeshRenderer& renderer, DirectX::FXMMATRIX transform)
    {
        const auto maxScaleExtent = GetMaxScaleExtent(transform);
        const auto maxMeshExtent = renderer.GetMesh().maxExtent;
        Sphere sphere{.center = Vector3::Zero(), .radius = maxScaleExtent * maxMeshExtent};
        DirectX::XMStoreVector3(&sphere.center, transform.r[3]);
        return physics::Intersect(frustum, sphere);
    }

    /** @todo This is crap. We need a hash or id for materials. */
    bool BelongsToBatch(const MeshRenderer& renderer, const graphics::Batch* batch)
    {
        if(renderer.GetMesh().firstIndex != batch->mesh.firstIndex)
            return false;

        const auto& textures = renderer.GetTextureIndices();

        if(textures.baseColor.index != batch->textures.baseColor.index)
            return false;

        if(textures.normal.index != batch->textures.normal.index)
            return false;

        if(textures.roughness.index != batch->textures.roughness.index)
            return false;

        if(textures.metallic.index != batch->textures.metallic.index)
            return false;

        return true;
    }
}

namespace nc::graphics
{
    MeshBatcher::MeshBatcher(Registry* registry)
        : m_registry{registry},
          m_batches{},
          m_isDirty{true}
    {
        /** @todo Add these once this is incorporated and update StoragePolicy<MeshRenderer>. */
        // m_registry->RegisterOnAddCallback<MeshRenderer>([this](MeshRenderer&) { this->m_isDirty = true; });
        // m_registry->RegisterOnRemoveCallback<MeshRenderer>([this](Entity) { this->m_isDirty = true; });
    }

    auto MeshBatcher::BuildBatches(const Frustum& cameraFrustum) -> std::span<const Batch>
    {
        if(m_isDirty)
        {
            Sort();
        }

        m_batches.clear();
        Batch* current = nullptr;

        for(const auto& renderer : m_registry->ViewAll<MeshRenderer>())
        {
            const auto& modelMatrix = m_registry->Get<Transform>(renderer.ParentEntity())->TransformationMatrix();

            if(!IsViewedByFrustum(cameraFrustum, renderer, modelMatrix))
            {
                continue;
            }

            if(current && BelongsToBatch(renderer, current))
            {
                ++current->count;
                continue;
            }

            current = AddNewBatch(renderer);
        }

        return std::span<const Batch>{m_batches};
    }

    void MeshBatcher::Clear() noexcept
    {
        m_batches.clear();
        m_batches.shrink_to_fit();
    }

    void MeshBatcher::Sort()
    {
        /** @todo Comparison should also deal with materials. */
        m_isDirty = false;
        m_registry->Sort<MeshRenderer>([](const auto& lhs, const auto& rhs)
        {
            return lhs.GetMesh().firstIndex < rhs.GetMesh().firstIndex;
        });
    }

    auto MeshBatcher::AddNewBatch(const MeshRenderer& renderer) -> Batch*
    {
        return &m_batches.emplace_back(renderer.GetMesh(), renderer.GetTextureIndices(), 1u);
    }
}