#include "PerFrameResourceSignature.h"
#include "EnvironmentBufferResource.h"
#include "TextureBufferResource.h"
#include "WireframeBufferResource.h"
#include "PostProcessPropertyBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
PerFrameResourceSignature::PerFrameResourceSignature(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     std::string_view signatureName,
                                                     uint8_t bindingIndex,
                                                     const StructuredBufferDesc& transformResourceDesc,
                                                     const StructuredBufferDesc& staticMeshInstanceResourceDesc,
                                                     const StructuredBufferDesc& skinnedMeshInstanceResourceDesc,
                                                     const StructuredBufferDesc& lightResourceDesc,
                                                     const StructuredBufferDesc& materialResourceDesc,
                                                     const StructuredBufferDesc& boneResourceDesc,
                                                     const StructuredBufferDesc& particleResourceDesc,
                                                     const TextureBufferDesc& textureResourceDesc,
                                                     const UniformBufferDesc& environmentResourceDesc,
                                                     const UniformBufferDesc& wireframeResourceDesc,
                                                     const UniformBufferDesc& outlinePassPropertiesDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(transformResourceDesc),
        ToPipelineResourceDesc(staticMeshInstanceResourceDesc),
        ToPipelineResourceDesc(skinnedMeshInstanceResourceDesc),
        ToPipelineResourceDesc(lightResourceDesc),
        ToPipelineResourceDesc(materialResourceDesc),
        ToPipelineResourceDesc(boneResourceDesc),
        ToPipelineResourceDesc(particleResourceDesc),
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

    m_lightDataResource = std::make_unique<StructuredBuffer<LightData>>
    (
        context,
        device,
        GetVariable(lightResourceDesc, m_srb),
        lightResourceDesc
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

    m_particleDataResource = std::make_unique<StructuredBuffer<ParticleData>>(
        context,
        device,
        GetVariable(particleResourceDesc, m_srb),
        particleResourceDesc
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
        context,
        device,
        GetVariable(outlinePassPropertiesDesc.shaderType, outlinePassPropertiesDesc.resourceKey.data(), m_srb)
    );
}

PerFrameResourceSignature::~PerFrameResourceSignature() noexcept = default;

} // namespace nc::graphics
