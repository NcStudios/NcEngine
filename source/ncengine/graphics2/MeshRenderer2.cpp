#include "ncengine/graphics/MeshRenderer2.h"
#include "MeshRendererContext.h"

namespace nc
{
MeshRenderer2::MeshRenderer2(Entity self,
                             const asset::MeshView& mesh,
                             const MaterialDesc& materialDesc)
    : m_self{self},
      m_meshId{mesh.id},
      m_material{MaterialInstance{materialDesc}}
{
    const auto instance = s_ctx->instanceCache.AddInstance(self, m_material.GetHandle());
    s_ctx->passCache.AddStaticTarget(
        materialDesc.passes,
        self.Index(),
        instance,
        mesh
    );
}

MeshRenderer2::~MeshRenderer2() noexcept
{
    if (m_self.Valid())
    {
        s_ctx->instanceCache.RemoveInstance(m_self);
        s_ctx->passCache.RemoveStaticTarget(m_material.GetPasses(), m_self.Index());
    }
}

void MeshRenderer2::SetMesh(const asset::MeshView& mesh)
{
    m_meshId = mesh.id;
    s_ctx->passCache.UpdateStaticTargetMesh(
        m_material.GetPasses(),
        m_self.Index(),
        mesh
    );
}
} // namespace nc
