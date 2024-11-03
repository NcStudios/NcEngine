#include "ComponentResourceSignature.h"

#include <array>

namespace nc::graphics
{
ComponentResourceSignature::ComponentResourceSignature(Diligent::IRenderDevice& device,
                                                       Diligent::IDeviceContext& context,
                                                       uint32_t maxMeshRenderers)
{
    const auto resources = std::array
    {
        MeshRendererBufferResource::MakeResourceDesc(MeshRendererShaderVariableName, maxMeshRenderers)
    };

    auto desc = Diligent::PipelineResourceSignatureDesc{};
    desc.Name = SignatureName;
    desc.Resources = resources.data(),
    desc.NumResources = static_cast<uint32_t>(resources.size()),
    desc.BindingIndex = BindingIndex,
    device.CreatePipelineResourceSignature(desc, &m_signature);

    if (!m_signature)
    {
        throw NcError("Failed to create resource signature.");
    }

    m_signature->CreateShaderResourceBinding(&m_srb);
    if (!m_srb)
    {
        throw NcError{"Failed to create shader resource binding"};
    }

    m_meshRendererResource = std::make_unique<MeshRendererBufferResource>
    (
        GetVariable(Diligent::SHADER_TYPE_PIXEL, MeshRendererShaderVariableName),
        device,
        maxMeshRenderers
    );
}

ComponentResourceSignature::~ComponentResourceSignature() noexcept = default;

auto ComponentResourceSignature::GetVariable(Diligent::SHADER_TYPE shaderType, const char* name) -> Diligent::IShaderResourceVariable&
{
    auto var = m_srb->GetVariableByName(shaderType, name);
    if (!var)
    {
        throw NcError{fmt::format("Failed retrieving shader variable '{}'", name)};
    }

    return *var;
}
} // namespace nc::graphics
