#include "ShaderBindings.h"
#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
void ShaderBindings::Update(Diligent::IDeviceContext& context,
                            Diligent::IRenderDevice& device,
                            const FrontendRenderState& renderState)
{
    NC_PROFILE_SCOPE("ShaderBindings::Update()", ProfileCategory::Rendering);
    m_globalSignature.GetGlobalEnvironment().Update(renderState.cameraState, context);

    /** @todo #794 Once plumbing is finalized with material pass implementation, make sure this is
     *             only sending dirty items. */
    const auto& instanceData = renderState.meshRendererState.instanceData;
    if (!instanceData.instances.empty())
    {
        m_componentSignature.GetMeshRendererBuffer().Update(
            context,
            device,
            instanceData
        );
    }

    const auto& materialData = renderState.materialRenderState;
    if (!materialData.instances.empty())
    {
        m_materialSignature.GetMaterialDataResource().Update(
            context,
            device,
            materialData
        );
    }
}
} // namespace nc::graphics
