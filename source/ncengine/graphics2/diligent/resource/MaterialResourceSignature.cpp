#include "MaterialResourceSignature.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
MaterialResourceSignature::MaterialResourceSignature(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     std::string_view signatureName,
                                                     std::string_view materialPropertiesVariableName,
                                                     uint8_t bindingIndex,
                                                     uint32_t maxInstances)
{
    const auto resource = MaterialPropertiesBufferResource::MakeResourceDesc(materialPropertiesVariableName);
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

    auto variable = m_srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, materialPropertiesVariableName.data());
    if (!variable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", materialPropertiesVariableName)};
    }

    m_materialPropertiesResource = std::make_unique<MaterialPropertiesBufferResource>(
        context,
        device,
        *variable,
        maxInstances
    );
}
} // namespace nc::graphics
