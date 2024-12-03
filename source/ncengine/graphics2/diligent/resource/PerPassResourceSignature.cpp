#include "PerPassResourceSignature.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
PerPassResourceSignature::PerPassResourceSignature(Diligent::IRenderDevice& device,
                                                   std::string_view signatureName,
                                                   uint8_t bindingIndex,
                                                   const TextureBufferResourceDesc& postProcessResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(postProcessResourceDesc)
    };

    const auto sampler = PostProcessSinkBufferResource::MakeSamplerDesc(postProcessResourceDesc.resourceKey);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = resources.data();
    desc.NumResources = static_cast<uint32_t>(resources.size());
    desc.ImmutableSamplers = &sampler,
    desc.NumImmutableSamplers = 1,
    desc.UseCombinedTextureSamplers = true,
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

    m_postProcessBufferResource = std::make_unique<PostProcessSinkBufferResource>(
        GetVariable(postProcessResourceDesc.shaderType, postProcessResourceDesc.resourceKey.data(), m_srb),
        postProcessResourceDesc.maxElementCount
    );
}
} // namespace nc::graphics
