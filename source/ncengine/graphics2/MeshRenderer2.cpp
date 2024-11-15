#include "ncengine/graphics/MeshRenderer2.h"
#include "frontend/subsystem/MeshRendererSubsystem.h"

namespace nc
{
MeshRenderer2::MeshRenderer2(Entity self,
                             const asset::MeshView& mesh,
                             const MaterialDesc& materialDesc)
    : m_self{self},
      m_meshId{mesh.id},
      m_instance{0},
      m_material{MaterialInstance{materialDesc}}
{
    m_instance = s_subsystem->AddInstance(
        self,
        m_material.GetHandle(),
        materialDesc.passes,
        mesh
    );
}

void MeshRenderer2::SetMesh(const asset::MeshView& mesh)
{
    m_meshId = mesh.id;
    s_subsystem->SetInstanceMesh(m_self, m_material.GetPasses(), mesh);
}

void MeshRenderer2::Release() noexcept
{
    if (m_self.Valid())
    {
        s_subsystem->RemoveInstance(
            m_self,
            m_instance,
            m_material.GetPasses()
        );
    }
}
} // namespace nc
