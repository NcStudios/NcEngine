#include "GlobalResourceSignature.h"
#include "GlobalTextureBufferResource.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
GlobalResourceSignature::GlobalResourceSignature(Diligent::IRenderDevice& device, uint32_t maxTextures)
{
    const auto resource = GlobalTextureBufferResource::MakeResourceDesc(GlobalTextureBufferShaderVariableName, maxTextures);
    const auto sampler = GlobalTextureBufferResource::MakeSamplerDesc(GlobalTextureBufferShaderVariableName);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = SignatureName;
    desc.Resources = &resource,
    desc.NumResources = 1,
    desc.ImmutableSamplers = &sampler,
    desc.NumImmutableSamplers = 1,
    desc.BindingIndex = BindingIndex,
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

    auto textureVariable = m_srb->GetVariableByName(Diligent::SHADER_TYPE_PIXEL, GlobalTextureBufferShaderVariableName);
    if (!textureVariable)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", GlobalTextureBufferShaderVariableName)};
    }

    m_textureResource = std::make_unique<GlobalTextureBufferResource>(*textureVariable, maxTextures);
}
} // namespace nc::graphics
