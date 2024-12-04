#include "ncengine/graphics/MeshRenderer2.h"
#include "frontend/subsystem/MeshRendererSubsystem.h"

namespace nc
{
MeshRenderer2::MeshRenderer2(Entity self,
                             const asset::MeshView& mesh,
                             const MaterialDesc& materialDesc)
    : m_self{self},
      m_meshId{mesh.id},
      m_transformDataHandle{0},
      m_material{MaterialInstance{materialDesc}}
{
    m_transformDataHandle = s_subsystem->AddInstance(
        self,
        m_material.GetHandle(),
        materialDesc.passes,
        mesh
    );
}

void MeshRenderer2::SetMesh(const asset::MeshView& mesh)
{
    s_subsystem->SetInstanceMesh(
        m_self,
        m_transformDataHandle,
        m_material.GetHandle(),
        m_material.GetPasses(),
        m_meshId,
        mesh
    );

    m_meshId = mesh.id;
}

void MeshRenderer2::SetMaterial(const MaterialDesc& materialDesc)
{
    const auto currentPasses = m_material.GetPasses();
    m_material = MaterialInstance{materialDesc};
    s_subsystem->SetInstanceMaterial(
        m_self,
        m_transformDataHandle,
        m_material.GetHandle(),
        currentPasses,
        m_material.GetPasses(),
        m_meshId
    );
}

void MeshRenderer2::Release() noexcept
{
    if (m_self.Valid())
    {
        s_subsystem->RemoveInstance(
            m_self,
            m_transformDataHandle,
            m_meshId,
            m_material.GetPasses()
        );
    }
}
} // namespace nc
