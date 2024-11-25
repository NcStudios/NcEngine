#include "ShaderBindings.h"
#include "EnvironmentBufferResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

namespace nc::graphics
{
void ShaderBindings::Update(Diligent::IDeviceContext& context,
                            Diligent::IRenderDevice& device,
                            const FrontendRenderState& renderState)
{
    m_perFrameSignature.GetEnvironmentBuffer().Update(context, renderState.cameraState, renderState.lightRenderState);

    /** @todo #794 Once plumbing is finalized with material pass implementation, make sure this is
     *             only sending dirty items. */
    const auto& instanceData = renderState.meshRendererState.instanceData;
    if (!instanceData.empty())
    {
        auto meshBufferUpdateInfo = BufferUpdateInfo<MeshRendererData>{
            .instances = instanceData,
            .dirtyRanges = {{0, instanceData.size() }}
        };

        m_perFrameSignature.GetMeshRendererBuffer().Update(context, device, meshBufferUpdateInfo);
    }

    const auto& dirLightData = renderState.lightRenderState.directionalLights;
    if (!dirLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<DirectionalLightData>{
            .instances = dirLightData,
            .dirtyRanges = {{0, dirLightData.size()}}
        };
        m_perFrameSignature.GetDirectionaLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& pointLightData = renderState.lightRenderState.pointLights;
    if (!pointLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<PointLightData>{
            .instances = pointLightData,
            .dirtyRanges = {{0, pointLightData.size()}}
        };
        m_perFrameSignature.GetPointLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& spotLightData = renderState.lightRenderState.spotLights;
    if (!spotLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<SpotLightData>{
            .instances = spotLightData,
            .dirtyRanges = {{0, spotLightData.size()}}
        };
        m_perFrameSignature.GetSpotLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& materialData = renderState.materialRenderState;
    if (!materialData.instances.empty())
    {
        m_perFrameSignature.GetMaterialDataResource().Update(
            context,
            device,
            materialData
        );
    }
}
} // namespace nc::graphics
