#include "ComponentResourceSignature.h"

#include <array>

namespace nc::graphics
{
ComponentResourceSignature::ComponentResourceSignature(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       std::string_view signatureName,
                                                       uint8_t bindingIndex,
                                                       const StructuredBufferResourceDesc& transformResourceDesc,
                                                       const StructuredBufferResourceDesc& instanceResourceDesc,
                                                       const StructuredBufferResourceDesc& directionalLightResourceDesc,
                                                       const StructuredBufferResourceDesc& pointLightResourceDesc,
                                                       const StructuredBufferResourceDesc& spotLightResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(transformResourceDesc),
        ToPipelineResourceDesc(instanceResourceDesc),
        ToPipelineResourceDesc(directionalLightResourceDesc),
        ToPipelineResourceDesc(pointLightResourceDesc),
        ToPipelineResourceDesc(spotLightResourceDesc)
    };

    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = resources.data(),
    desc.NumResources = static_cast<uint32_t>(resources.size()),
    desc.BindingIndex = bindingIndex,
    device.CreatePipelineResourceSignature(desc, &m_signature);

    if (!m_signature)
    {
        throw NcError("Failed to create resource signature.");
    }

    m_signature->CreateShaderResourceBinding(&m_srb);
    if (!m_srb)
    {
        throw NcError{"Failed to create shader resource binding"};
    }

    m_transformBuffer = std::make_unique<StructuredBuffer<TransformData>>
    (
        context,
        device,
        GetVariable(transformResourceDesc.shaderType, transformResourceDesc.resourceKey.data(), m_srb),
        transformResourceDesc
    );

    m_instanceBuffer = std::make_unique<StructuredBuffer<InstanceData>>
    (
        context,
        device,
        GetVariable(instanceResourceDesc.shaderType, instanceResourceDesc.resourceKey.data(), m_srb),
        instanceResourceDesc
    );

    m_directionalLightResource = std::make_unique<StructuredBuffer<DirectionalLightData>>
    (
        context,
        device,
        GetVariable(directionalLightResourceDesc.shaderType, directionalLightResourceDesc.resourceKey.data(), m_srb),
        directionalLightResourceDesc
    );

    m_pointLightResource = std::make_unique<StructuredBuffer<PointLightData>>
    (
        context,
        device,
        GetVariable(pointLightResourceDesc.shaderType, pointLightResourceDesc.resourceKey.data(), m_srb),
        pointLightResourceDesc
    );

    m_spotLightResource = std::make_unique<StructuredBuffer<SpotLightData>>
    (
        context,
        device,
        GetVariable(spotLightResourceDesc.shaderType, spotLightResourceDesc.resourceKey.data(), m_srb),
        spotLightResourceDesc
    );
}
} // namespace nc::graphics
