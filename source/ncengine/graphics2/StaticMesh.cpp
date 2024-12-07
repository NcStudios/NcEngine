#include "ncengine/graphics/StaticMesh.h"
#include "frontend/subsystem/MeshSubsystem.h"

namespace nc
{
StaticMesh::StaticMesh(Entity self,
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

void StaticMesh::SetMesh(const asset::MeshView& mesh)
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

void StaticMesh::SetMaterial(const MaterialDesc& materialDesc)
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

void StaticMesh::Release() noexcept
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
