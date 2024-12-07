#include "ncengine/graphics/Mesh.h"
#include "frontend/subsystem/MeshSubsystem.h"

namespace nc
{
MeshBase::MeshBase(Entity self,
                   const asset::MeshView& meshAsset,
                   const MaterialDesc& materialDesc,
                   MeshInstanceType type)
    : m_ctx{self, meshAsset.id, 0, 0, type},
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
} // namespace nc

