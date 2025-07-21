#include "ncengine/graphics/Mesh.h"
#include "frontend/subsystem/MeshSubsystem.h"
#include "ncasset/Assets.h"
#include "asset/AssetService.h"

namespace nc
{
MeshBase::MeshBase(Entity self,
                   const asset::MeshView& meshAsset,
                   const MaterialDesc& materialDesc,
                   asset::AssetId shapeKeyAnimationId,
                   MeshInstanceType type)
    : m_ctx{self, meshAsset.id, std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max(), shapeKeyAnimationId == asset::NullAssetId ? std::numeric_limits<uint32_t>::max() : asset::AssetService<asset::ShapeKeyAnimationView>::Get()->Acquire(shapeKeyAnimationId).index, meshAsset.firstVertex, type},
      m_material{MaterialInstance{materialDesc}}
{
    s_subsystem->AddInstance(m_ctx, m_material, meshAsset);
}

void MeshBase::SetMesh(const asset::MeshView& meshAsset)
{
    s_subsystem->SetInstanceMesh(m_ctx, m_material, meshAsset);
    m_ctx.meshId = meshAsset.id;
}

void MeshBase::SetMaterial(const MaterialDesc& materialDesc)
{
    const auto previousPasses = m_material.GetPasses();
    m_material = MaterialInstance{materialDesc};
    s_subsystem->SetInstanceMaterial(m_ctx, m_material, previousPasses);
}

void MeshBase::Release() noexcept
{
    if (m_ctx.entity.Valid())
    {
        s_subsystem->RemoveInstance(m_ctx, m_material);
    }
}

void StaticMesh::SetShapeKeyAnimation(asset::AssetId shapeKeyAnimationId)
{
    auto& ctx = GetMeshContext();
    ctx.shapeKeyDataHandle = shapeKeyAnimationId == asset::NullAssetId ? std::numeric_limits<uint32_t>::max() : asset::AssetService<asset::ShapeKeyAnimationView>::Get()->Acquire(shapeKeyAnimationId).index;
    auto* subsystem = GetSubsystem();
    auto& material = GetMaterial();
    subsystem->SetInstanceShapeKeyAnimation(ctx, material);
}

void SkinnedMesh::SetMesh(const asset::MeshView& meshAsset)
{
    MeshBase::SetMesh(meshAsset);
    m_skeletalAnimationcontroller.RefreshAnimation();
}
} // namespace nc

