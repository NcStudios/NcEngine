#include "ComponentResourceSignature.h"

#include <array>

namespace nc::graphics
{
ComponentResourceSignature::ComponentResourceSignature(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       std::string_view signatureName,
                                                       uint8_t bindingIndex,
                                                       const StructuredBufferResourceDesc& transformResourceDesc,
                                                       const StructuredBufferResourceDesc& instanceResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(transformResourceDesc),
        ToPipelineResourceDesc(instanceResourceDesc)
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

    auto transformVariable = m_srb->GetVariableByName(ToCommonShaderType(transformResourceDesc.shaderType), transformResourceDesc.resourceKey.data());
    if (!transformVariable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", transformResourceDesc.resourceKey)};
    }

    m_transformBuffer = std::make_unique<StructuredBuffer<TransformData>>
    (
        context,
        device,
        transformResourceDesc.resourceKey,
        *transformVariable,
        transformResourceDesc.maxElementCount,
        transformResourceDesc.maxElementCount
    );

    auto instanceVariable = m_srb->GetVariableByName(ToCommonShaderType(instanceResourceDesc.shaderType), instanceResourceDesc.resourceKey.data());
    if (!instanceVariable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", instanceResourceDesc.resourceKey)};
    }

    m_instanceBuffer = std::make_unique<StructuredBuffer<InstanceData>>
    (
        context,
        device,
        instanceResourceDesc.resourceKey,
        *instanceVariable,
        instanceResourceDesc.maxElementCount,
        instanceResourceDesc.maxElementCount
    );
}

} // namespace nc::graphics
