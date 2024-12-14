#include "PerFrameResourceSignature.h"
#include "EnvironmentBufferResource.h"
#include "PostProcessPropertyBufferResource.h"
#include "TextureBufferResource.h"
#include "WireframeBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
PerFrameResourceSignature::PerFrameResourceSignature(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     std::string_view signatureName,
                                                     uint8_t bindingIndex,
                                                     const StructuredBufferResourceDesc& transformResourceDesc,
                                                     const StructuredBufferResourceDesc& staticMeshInstanceResourceDesc,
                                                     const StructuredBufferResourceDesc& skinnedMeshInstanceResourceDesc,
                                                     const StructuredBufferResourceDesc& directionalLightResourceDesc,
                                                     const StructuredBufferResourceDesc& pointLightResourceDesc,
                                                     const StructuredBufferResourceDesc& spotLightResourceDesc,
                                                     const StructuredBufferResourceDesc& materialResourceDesc,
                                                     const StructuredBufferResourceDesc& boneResourceDesc,
                                                     const TextureBufferResourceDesc& textureResourceDesc,
                                                     const UniformBufferResourceDesc& environmentResourceDesc,
                                                     const UniformBufferResourceDesc& wireframeResourceDesc,
                                                     const UniformBufferResourceDesc& outlinePassPropertiesDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(transformResourceDesc),
        ToPipelineResourceDesc(staticMeshInstanceResourceDesc),
        ToPipelineResourceDesc(skinnedMeshInstanceResourceDesc),
        ToPipelineResourceDesc(directionalLightResourceDesc),
        ToPipelineResourceDesc(pointLightResourceDesc),
        ToPipelineResourceDesc(spotLightResourceDesc),
        ToPipelineResourceDesc(materialResourceDesc),
        ToPipelineResourceDesc(boneResourceDesc),
        ToPipelineResourceDesc(textureResourceDesc),
        ToPipelineResourceDesc(environmentResourceDesc),
        ToPipelineResourceDesc(wireframeResourceDesc),
        ToPipelineResourceDesc(outlinePassPropertiesDesc)
    };

    const auto sampler = TextureBufferResource::MakeSamplerDesc(textureResourceDesc.resourceKey);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = resources.data(),
    desc.NumResources = static_cast<uint32_t>(resources.size()),
    desc.ImmutableSamplers = &sampler,
    desc.NumImmutableSamplers = 1,
    desc.BindingIndex = bindingIndex,
    desc.UseCombinedTextureSamplers = true,
    device.CreatePipelineResourceSignature(desc, &m_signature);

    if (!m_signature)
    {
        throw NcError{"Failed to create resource signature"};
    }

    m_signature->CreateShaderResourceBinding(&m_srb);
    if (!m_srb)
    {
        throw NcError{"Failed to create shader resource binding"};
    }

    m_transformResource = std::make_unique<StructuredBuffer<TransformData>>
    (
        context,
        device,
        GetVariable(transformResourceDesc, m_srb),
        transformResourceDesc
    );

    m_staticMeshInstanceResource = std::make_unique<StructuredBuffer<StaticMeshInstanceData>>
    (
        context,
        device,
        GetVariable(staticMeshInstanceResourceDesc, m_srb),
        staticMeshInstanceResourceDesc
    );

    m_skinnedMeshInstanceResource = std::make_unique<StructuredBuffer<SkinnedMeshInstanceData>>
    (
        context,
        device,
        GetVariable(skinnedMeshInstanceResourceDesc, m_srb),
        skinnedMeshInstanceResourceDesc
    );

    m_directionalLightResource = std::make_unique<StructuredBuffer<DirectionalLightData>>
    (
        context,
        device,
        GetVariable(directionalLightResourceDesc, m_srb),
        directionalLightResourceDesc
    );

    m_pointLightResource = std::make_unique<StructuredBuffer<PointLightData>>
    (
        context,
        device,
        GetVariable(pointLightResourceDesc, m_srb),
        pointLightResourceDesc
    );

    m_spotLightResource = std::make_unique<StructuredBuffer<SpotLightData>>
    (
        context,
        device,
        GetVariable(spotLightResourceDesc, m_srb),
        spotLightResourceDesc
    );

    m_materialDataResource = std::make_unique<StructuredBuffer<MaterialData>>(
        context,
        device,
        GetVariable(materialResourceDesc, m_srb),
        materialResourceDesc
    );

    m_boneDataResource = std::make_unique<StructuredBuffer<BoneData>>(
        context,
        device,
        GetVariable(boneResourceDesc, m_srb),
        boneResourceDesc
    );

    m_textureResource = std::make_unique<TextureBufferResource>(
        GetVariable(textureResourceDesc, m_srb),
        textureResourceDesc.maxElementCount
    );

    m_environmentResource = std::make_unique<EnvironmentBufferResource>(
        context,
        device,
        GetVariable(environmentResourceDesc, m_srb)
    );

    m_wireframeBufferResource = std::make_unique<WireframeBufferResource>(
        context,
        device,
        GetVariable(wireframeResourceDesc, m_srb)
    );

    m_postProcessPropertyResource = std::make_unique<PostProcessPropertyBufferResource>(
        std::vector<PostProcessDataVariable>{
            PostProcessDataVariable{
                &GetVariable(outlinePassPropertiesDesc, m_srb),
                PostProcessPassFlag::Outline
            }
        }
    );
}

PerFrameResourceSignature::~PerFrameResourceSignature() noexcept = default;

} // namespace nc::graphics
