#include "ncengine/graphics/MeshRenderer2.h"
#include "frontend/subsystem/MeshRendererSubsystem.h"

namespace nc
{
MeshRenderer2::MeshRenderer2(Entity self,
                             const asset::MeshView& mesh,
                             const MaterialDesc& materialDesc)
    : m_self{self},
      m_meshId{mesh.id},
      m_transformIndex{0},
      m_material{MaterialInstance{materialDesc}}
{
    m_transformIndex = s_subsystem->AddInstance(
        self,
        m_material.GetHandle(),
        materialDesc.passes,
        mesh
    );
}

void MeshRenderer2::SetMesh(const asset::MeshView& mesh)
{
    const auto passes = m_material.GetPasses();
    s_subsystem->SetInstanceMesh(
        m_self,
        m_transformIndex,
        m_material.GetHandle(),
        passes,
        passes,
        m_meshId,
        mesh
    );

    m_meshId = mesh.id;
}

void MeshRenderer2::Release() noexcept
{
    if (m_self.Valid())
    {
        s_subsystem->RemoveInstance(
            m_self,
            m_transformIndex,
            m_meshId,
            m_material.GetPasses()
        );
    }
}
} // namespace nc
