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
                                                   const SinkBufferDesc& colorSinksDesc,
                                                   const SinkBufferDesc& depthSinksDesc,
                                                   const SinkBufferDesc& postProcessSinksDesc,
                                                   const SinkBufferDesc& uniShadowMapSinksDesc,
                                                   const SinkBufferDesc& pointShadowMapSinksDesc,
                                                   const UniformBufferDesc& postProcessPassPropertiesDesc,
                                                   const UniformBufferDesc& sinkIndexDesc)
    : m_postProcessSinkCount{postProcessSinksDesc.maxElementCount},
      m_postProcessResourceKey{postProcessSinksDesc.resourceKey}
{
    auto postProcessTexDesc = SinkBufferDesc{postProcessSinksDesc.resourceKey, postProcessSinksDesc.shaderType, 1, postProcessSinksDesc.dynamic};

    const auto resources = std::array{
        ToPipelineResourceDesc(colorSinksDesc),
        ToPipelineResourceDesc(depthSinksDesc),
        ToPipelineResourceDesc(postProcessTexDesc), // Even though we have multiple post process resources, we only ever bind one at a time.
        ToPipelineResourceDesc(sinkIndexDesc),
        ToPipelineResourceDesc(postProcessPassPropertiesDesc),
        ToPipelineResourceDesc(uniShadowMapSinksDesc),
        ToPipelineResourceDesc(pointShadowMapSinksDesc)
    };

    const auto samplers = std::array{
        SinkBufferResource::MakeSamplerDesc(colorSinksDesc.resourceKey),
        SinkBufferResource::MakeShadowSamplerDesc(uniShadowMapSinksDesc.resourceKey),
    };

    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = resources.data();
    desc.NumResources = static_cast<uint32_t>(resources.size());
    desc.ImmutableSamplers = samplers.data(),
    desc.NumImmutableSamplers = static_cast<uint32_t>(samplers.size()),
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

    m_colorSinksResource = std::make_unique<SinkBufferResource>(
        GetVariable(colorSinksDesc.shaderType, colorSinksDesc.resourceKey.data(), m_srb),
        MakeColorSinkBufferDesc(colorSinksDesc.maxElementCount)
    );
    m_colorSinksResource->Update();

    m_depthSinksResource = std::make_unique<SinkBufferResource>(
        GetVariable(depthSinksDesc.shaderType, depthSinksDesc.resourceKey.data(), m_srb),
        MakeDepthSinkBufferDesc(depthSinksDesc.maxElementCount)
    );
    m_depthSinksResource->Update();

    m_sinkIndexBufferResource = std::make_unique<SinkIndexBufferResource>(
        context, device,
        GetVariable(sinkIndexDesc.shaderType, sinkIndexDesc.resourceKey.data(), m_srb)
    );

    m_postProcessSinksResource.reserve(postProcessSinksDesc.maxElementCount);
    for (auto i = 0u; i < postProcessSinksDesc.maxElementCount; i++)
    {
        m_postProcessSinksResource.emplace_back(
            GetVariable( Diligent::SHADER_TYPE_PIXEL, m_postProcessResourceKey.data(), m_srb),
            MakeColorSinkBufferDesc(1));
    }

    m_postProcessPropertyResource = std::make_unique<PostProcessPropertyBufferResource>(
        context,
        device,
        GetVariable(postProcessPassPropertiesDesc.shaderType, postProcessPassPropertiesDesc.resourceKey.data(), m_srb)
    );

    m_uniShadowMapSinksResource = std::make_unique<SinkBufferResource>(
        GetVariable(uniShadowMapSinksDesc.shaderType, uniShadowMapSinksDesc.resourceKey.data(), m_srb),
        MakeShadowSinkBufferDesc(uniShadowMapSinksDesc.maxElementCount)
    );
    m_uniShadowMapSinksResource->Update();

    m_pointShadowMapSinksResource = std::make_unique<SinkBufferResource>(
        GetVariable(pointShadowMapSinksDesc.shaderType, pointShadowMapSinksDesc.resourceKey.data(), m_srb),
        MakeShadowSinkBufferDesc(pointShadowMapSinksDesc.maxElementCount)
    );
    m_pointShadowMapSinksResource->Update();
}

void PerPassResourceSignature::BindPostProcessSink(uint32_t index)
{
    GetPostProcessSinkResource(index).Update();
}

PerPassResourceSignature::~PerPassResourceSignature() = default;
} // namespace nc::graphics
