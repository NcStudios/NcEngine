#include "ShaderBindings.h"
#include "GlobalEnvironmentResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

namespace nc::graphics
{
void ShaderBindings::Update(Diligent::IDeviceContext& context,
                    Diligent::IRenderDevice& device,
                    const FrontendRenderState& renderState)
{
    m_globalSignature.GetGlobalEnvironment().Update(renderState.cameraState, context);

    /** @todo #794 Once plumbing is finalized with material pass implementation, make sure this is
     *             only sending dirty items. */
    auto meshBufferUpdateInfo = BufferUpdateInfo<MeshRendererData>{
        .instances = renderState.meshRendererState.modelMatrices,
        .dirtyRanges = {{0, renderState.meshRendererState.modelMatrices.size() }}
    };

    if (!meshBufferUpdateInfo.instances.empty())
    {
        m_componentSignature.GetMeshRendererBuffer().Update(context, device, meshBufferUpdateInfo);
    }
}
} // namespace nc::graphics
