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
    m_globalSignature.GetGlobalEnvironment().Update(context,
                                                    renderState.cameraState, 
                                                    renderState.lightRenderState);

    const auto& transformData = renderState.meshRendererState.transformData;
    if (!transformData.instances.empty())
    {
        m_componentSignature.GetTransformBuffer().Update(
            context,
            device,
            transformData
        );
    }

    const auto& instanceData = renderState.meshRendererState.instanceData;
    if (!instanceData.instances.empty())
    {
        m_componentSignature.GetInstanceBuffer().Update(
            context,
            device,
            instanceData
        );
    }

    const auto& dirLightData = renderState.lightRenderState.directionalLights;
    if (!dirLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<DirectionalLightData>{
            .instances = dirLightData,
            .dirtyRanges = {{0, dirLightData.size()}}
        };
        m_componentSignature.GetDirectionaLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& pointLightData = renderState.lightRenderState.pointLights;
    if (!pointLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<PointLightData>{
            .instances = pointLightData,
            .dirtyRanges = {{0, pointLightData.size()}}
        };
        m_componentSignature.GetPointLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& spotLightData = renderState.lightRenderState.spotLights;
    if (!spotLightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<SpotLightData>{
            .instances = spotLightData,
            .dirtyRanges = {{0, spotLightData.size()}}
        };
        m_componentSignature.GetSpotLightBuffer().Update(context, device, lightBufferUpdateInfo);
    }

    const auto& materialData = renderState.materialRenderState;
    if (!materialData.instances.empty())
    {
        NC_PROFILE_SCOPE("ShaderBindings::Update() - MaterialData", ProfileCategory::Rendering);
        m_materialSignature.GetMaterialDataResource().Update(
            context,
            device,
            materialData
        );
    }
}
} // namespace nc::graphics
