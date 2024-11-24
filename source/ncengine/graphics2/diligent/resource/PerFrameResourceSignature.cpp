#include "GlobalResourceSignature.h"
#include "GlobalEnvironmentResource.h"
#include "GlobalTextureBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
GlobalResourceSignature::GlobalResourceSignature(Diligent::IDeviceContext& context,
                                                 Diligent::IRenderDevice& device,
                                                 std::string_view signatureName,
                                                 uint8_t bindingIndex,
                                                 TextureBufferResourceDesc textureResourceDesc,
                                                 UniformBufferResourceDesc environmentResourceDesc)
{
    const auto resources = std::array{
        ToPipelineResourceDesc(textureResourceDesc),
        ToPipelineResourceDesc(environmentResourceDesc)
    };

    const auto sampler = GlobalTextureBufferResource::MakeSamplerDesc(textureResourceDesc.resourceKey);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = signatureName.data();
    desc.Resources = resources.data(),
    desc.NumResources = static_cast<uint32_t>(resources.size()),
    desc.ImmutableSamplers = &sampler,
    desc.NumImmutableSamplers = 1,
    desc.BindingIndex = bindingIndex,
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

    m_textureResource = std::make_unique<GlobalTextureBufferResource>(
        GetVariable(textureResourceDesc.shaderType, textureResourceDesc.resourceKey.data(), m_srb),
        textureResourceDesc.maxElementCount
    );

    m_environmentResource = std::make_unique<GlobalEnvironmentResource>(
        context,
        device,
        GetVariable(environmentResourceDesc.shaderType, environmentResourceDesc.resourceKey.data(), m_srb)
    );
}

GlobalResourceSignature::~GlobalResourceSignature() noexcept = default;

} // namespace nc::graphics
