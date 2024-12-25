#include "ShaderBindings.h"
#include "EnvironmentBufferResource.h"
#include "graphics2/diligent/resource/SinkBufferResource.h"
#include "graphics2/diligent/resource/SinkIndexBufferResource.h"
#include "graphics2/frontend/FrontendRenderState.h"

#include "ncengine/debug/Profile.h"

namespace nc::graphics
{
void ShaderBindings::Update(Diligent::IDeviceContext& context,
                            Diligent::IRenderDevice& device,
                            const FrontendRenderState& renderState)
{
    NC_PROFILE_SCOPE("ShaderBindings::Update()", ProfileCategory::Rendering);
    m_perFrameSignature.GetEnvironmentBuffer().Update(context, renderState.cameraState, renderState.lightRenderState);

    const auto& transformData = renderState.meshRenderState.transformData;
    if (!transformData.instances.empty())
    {
        m_perFrameSignature.GetTransformBuffer().Update(
            context,
            device,
            transformData
        );
    }

    const auto& staticMeshInstanceData = renderState.meshRenderState.staticMeshInstanceData;
    if (!staticMeshInstanceData.instances.empty())
    {
        m_perFrameSignature.GetStaticMeshInstanceBuffer().Update(
            context,
            device,
            staticMeshInstanceData
        );
    }

    const auto& skinnedMeshInstanceData = renderState.meshRenderState.skinnedMeshInstanceData;
    if (!skinnedMeshInstanceData.instances.empty())
    {
        m_perFrameSignature.GetSkinnedMeshInstanceBuffer().Update(
            context,
            device,
            skinnedMeshInstanceData
        );
    }

    const auto& particleData = renderState.particleRenderState.particleData;
    if (!particleData.instances.empty())
    {
        m_perFrameSignature.GetParticleDataBuffer().Update(
            context,
            device,
            particleData
        );
    }

    const auto& lightData = renderState.lightRenderState.lights;
    if (!lightData.empty())
    {
        auto lightBufferUpdateInfo = BufferUpdateInfo<LightData>{
            .instances = lightData,
            .dirtyRanges = {{0, lightData.size()}}
        };
        m_perFrameSignature.GetLightBuffer().Update(context, device, lightBufferUpdateInfo);
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

    const auto& boneData = renderState.animationRenderState.boneData;
    if (!boneData.instances.empty())
    {
        m_perFrameSignature.GetBoneDataResource().Update(
            context,
            device,
            boneData
        );
    }
}
} // namespace nc::graphics
