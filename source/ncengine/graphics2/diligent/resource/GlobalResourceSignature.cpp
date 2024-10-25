#include "GlobalResourceSignature.h"
#include "GlobalEnvironmentResource.h"
#include "GlobalTextureBufferResource.h"

#include "ncutility/NcError.h"

#include <array>

namespace nc::graphics
{
GlobalResourceSignature::GlobalResourceSignature(Diligent::IRenderDevice& device,
                                                 Diligent::IDeviceContext& context,
                                                 uint32_t maxTextures)
{
    const auto resources = std::array{
        GlobalTextureBufferResource::MakeResourceDesc(GlobalTextureBufferShaderVariableName, maxTextures),
        GlobalEnvironmentResource::MakeResourceDesc(GlobalEnvironmentShaderVariableName)
    };

    const auto sampler = GlobalTextureBufferResource::MakeSamplerDesc(GlobalTextureBufferShaderVariableName);
    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = SignatureName;
    desc.Resources = resources.data(),
    desc.NumResources = static_cast<uint32_t>(resources.size()),
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

    m_textureResource = std::make_unique<GlobalTextureBufferResource>(
        GetVariable(Diligent::SHADER_TYPE_PIXEL, GlobalTextureBufferShaderVariableName),
        maxTextures
    );

    m_environmentResource = std::make_unique<GlobalEnvironmentResource>(
        GetVariable(Diligent::SHADER_TYPE_PIXEL, GlobalEnvironmentShaderVariableName),
        device,
        context
    );
}

GlobalResourceSignature::~GlobalResourceSignature() noexcept = default;

auto GlobalResourceSignature::GetVariable(Diligent::SHADER_TYPE shaderType, const char* name) -> Diligent::IShaderResourceVariable&
{
    auto var = m_srb->GetVariableByName(shaderType, name);
    if (!var)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", name)};
    }

    return *var;
}
} // namespace nc::graphics
