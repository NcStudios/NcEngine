#include "PerPassResourceSignature.h"
#include "SinkBufferResource.h"
#include "SinkIndexBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
PerPassResourceSignature::PerPassResourceSignature(Diligent::IRenderDevice& device,
                                                   Diligent::IDeviceContext& context,
                                                   std::string_view signatureName,
                                                   uint8_t bindingIndex,
                                                   const TextureBufferResourceDesc& colorSinkResourceDesc,
                                                   const TextureBufferResourceDesc& depthSinkResourceDesc,
                                                   const UniformBufferResourceDesc& sinkIndexResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(colorSinkResourceDesc),
        ToPipelineResourceDesc(depthSinkResourceDesc),
        ToPipelineResourceDesc(sinkIndexResourceDesc)
    };

    const auto sampler = SinkBufferResource::MakeSamplerDesc(colorSinkResourceDesc.resourceKey);
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

    m_colorSinkBufferResource = std::make_unique<SinkBufferResource>(
        GetVariable(colorSinkResourceDesc.shaderType, colorSinkResourceDesc.resourceKey.data(), m_srb),
        MakeColorSinkBufferDesc(colorSinkResourceDesc.maxElementCount)
    );

    m_depthSinkBufferResource = std::make_unique<SinkBufferResource>(
        GetVariable(depthSinkResourceDesc.shaderType, depthSinkResourceDesc.resourceKey.data(), m_srb),
        MakeDepthSinkBufferDesc(depthSinkResourceDesc.maxElementCount)
    );

    m_sinkIndexBufferResource = std::make_unique<SinkIndexBufferResource>(
        context, device,
        GetVariable(sinkIndexResourceDesc.shaderType, sinkIndexResourceDesc.resourceKey.data(), m_srb)
    );
}
} // namespace nc::graphics
