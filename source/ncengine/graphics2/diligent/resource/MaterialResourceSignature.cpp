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

    m_materialDataResource = std::make_unique<StructuredBuffer<MaterialData>>(
        context,
        device,
        GetVariable(materialResourceDesc.shaderType, materialResourceDesc.resourceKey.data(), m_srb),
        materialResourceDesc
    );
}
} // namespace nc::graphics
