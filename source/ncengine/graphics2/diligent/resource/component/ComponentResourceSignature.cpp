#include "ComponentResourceSignature.h"

#include <array>

namespace nc::graphics
{
ComponentResourceSignature::ComponentResourceSignature(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       std::string_view signatureName,
                                                       std::string_view meshRendererBufferVariableName,
                                                       uint8_t bindingIndex,
                                                       uint32_t maxInstances)
{
    const auto resources = std::array
    {
        MeshRendererBufferResource::MakeResourceDesc(meshRendererBufferVariableName, maxInstances)
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

    auto variable = m_srb->GetVariableByName(Diligent::SHADER_TYPE_VERTEX, meshRendererBufferVariableName.data());
    if (!variable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", meshRendererBufferVariableName)};
    }

    m_meshRendererResource = std::make_unique<MeshRendererBufferResource>
    (
        context,
        device,
        *variable,
        maxInstances
    );
}

} // namespace nc::graphics
