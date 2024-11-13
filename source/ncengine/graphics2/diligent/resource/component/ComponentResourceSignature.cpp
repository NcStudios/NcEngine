#include "ComponentResourceSignature.h"

#include <array>

namespace nc::graphics
{
ComponentResourceSignature::ComponentResourceSignature(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       std::string_view signatureName,
                                                       uint8_t bindingIndex,
                                                       StructuredBufferResourceDesc meshRendererResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(meshRendererResourceDesc)
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

    auto variable = m_srb->GetVariableByName(ToCommonShaderType(meshRendererResourceDesc.shaderType), meshRendererResourceDesc.resourceKey.data());
    if (!variable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", meshRendererResourceDesc.resourceKey)};
    }

    m_meshRendererResource = std::make_unique<StructuredBuffer<MeshRendererData>>
    (
        context,
        device,
        meshRendererResourceDesc.resourceKey,
        *variable,
        meshRendererResourceDesc.maxElementCount,
        meshRendererResourceDesc.maxElementCount
    );
}

} // namespace nc::graphics
