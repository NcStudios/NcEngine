#include "MaterialResourceSignature.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
MaterialResourceSignature::MaterialResourceSignature(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     std::string_view signatureName,
                                                     std::string_view materialBufferVariableName,
                                                     uint8_t bindingIndex,
                                                     uint32_t maxInstanceCount,
                                                     uint32_t initialInstanceCount)
{
    const auto resource = StructuredBuffer<MaterialData>::MakeResourceDesc(materialBufferVariableName, Diligent::SHADER_TYPE_PIXEL);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = &resource;
    desc.NumResources = 1;
    desc.BindingIndex = bindingIndex;
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

    auto variable = m_srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, materialBufferVariableName.data());
    if (!variable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", materialBufferVariableName)};
    }

    m_materialDataResource = std::make_unique<StructuredBuffer<MaterialData>>(
        context,
        device,
        "MaterialDataBuffer",
        *variable,
        maxInstanceCount,
        initialInstanceCount
    );
}
} // namespace nc::graphics
