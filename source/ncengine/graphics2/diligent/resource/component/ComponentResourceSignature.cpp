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
        StructuredBuffer<MeshRendererData>::MakeResourceDesc(
            meshRendererResourceDesc.resourceKey,
            meshRendererResourceDesc.shaderType
        )
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

    /** While you have to specify both shader types (if using both) in the Resource Desc above, you can only specify pixel or vertex here. It doesn't matter which. 
     * Leaving this logic in instead of hard-coding because ResourceSignature is really starting to look like a class we may want to create a base for. */
    auto singleShaderType = meshRendererResourceDesc.shaderType == Diligent::SHADER_TYPE_VS_PS ? Diligent::SHADER_TYPE_VERTEX : meshRendererResourceDesc.shaderType;
    auto variable = m_srb->GetVariableByName(singleShaderType, meshRendererResourceDesc.resourceKey.data());
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
