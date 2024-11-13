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
    const auto& instanceData = renderState.meshRendererState.instanceData;
    if (!instanceData.empty())
    {
        auto meshBufferUpdateInfo = BufferUpdateInfo<MeshRendererData>{
            .instances = instanceData,
            .dirtyRanges = {{0, instanceData.size() }}
        };

        m_componentSignature.GetMeshRendererBuffer().Update(context, device, meshBufferUpdateInfo);
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
