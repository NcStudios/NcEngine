#include "PerPassResourceSignature.h"
#include "PostProcessColorSinkBufferResource.h"
#include "PostProcessDepthSinkBufferResource.h"
#include "PostProcessSinkIndexBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
PerPassResourceSignature::PerPassResourceSignature(Diligent::IRenderDevice& device,
                                                   Diligent::IDeviceContext& context,
                                                   std::string_view signatureName,
                                                   uint8_t bindingIndex,
                                                   const TextureBufferResourceDesc& postProcessColorSinkResourceDesc,
                                                   const TextureBufferResourceDesc& postProcessDepthSinkResourceDesc,
                                                   const UniformBufferResourceDesc& postProcessSinkIndexResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(postProcessColorSinkResourceDesc),
        ToPipelineResourceDesc(postProcessDepthSinkResourceDesc),
        ToPipelineResourceDesc(postProcessSinkIndexResourceDesc)
    };

    const auto sampler = PostProcessColorSinkBufferResource::MakeSamplerDesc(postProcessColorSinkResourceDesc.resourceKey);
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

    m_postProcessColorSinkBufferResource = std::make_unique<PostProcessColorSinkBufferResource>(
        GetVariable(postProcessColorSinkResourceDesc.shaderType, postProcessColorSinkResourceDesc.resourceKey.data(), m_srb),
        postProcessColorSinkResourceDesc.maxElementCount
    );

    m_postProcessDepthSinkBufferResource = std::make_unique<PostProcessDepthSinkBufferResource>(
        GetVariable(postProcessDepthSinkResourceDesc.shaderType, postProcessDepthSinkResourceDesc.resourceKey.data(), m_srb),
        postProcessDepthSinkResourceDesc.maxElementCount
    );

    m_postProcessSinkIndexBufferResource = std::make_unique<PostProcessSinkIndexBufferResource>(
        context, device,
        GetVariable(postProcessSinkIndexResourceDesc.shaderType, postProcessSinkIndexResourceDesc.resourceKey.data(), m_srb)
    );
}
} // namespace nc::graphics
