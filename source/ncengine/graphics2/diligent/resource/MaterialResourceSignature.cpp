#include "MaterialResourceSignature.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
MaterialResourceSignature::MaterialResourceSignature(Diligent::IDeviceContext& context,
                                                     Diligent::IRenderDevice& device,
                                                     std::string_view signatureName,
                                                     uint8_t bindingIndex,
                                                     StructuredBufferResourceDesc materialResourceDesc)
{
    const auto resource = ToPipelineResourceDesc(materialResourceDesc);

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

    auto variable = m_srb->GetVariableByName(ToCommonShaderType(materialResourceDesc.shaderType), materialResourceDesc.resourceKey.data());
    if (!variable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", materialResourceDesc.resourceKey)};
    }

    m_materialDataResource = std::make_unique<StructuredBuffer<MaterialData>>(
        context,
        device,
        materialResourceDesc.resourceKey,
        *variable,
        materialResourceDesc.maxElementCount,
        materialResourceDesc.initialElementCount
    );
}
} // namespace nc::graphics
