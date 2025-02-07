#include "TextureBufferResource.h"
#include "ResourceTypes.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"

namespace nc::graphics
{
TextureBufferResource::TextureBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures)
    : m_variable{&variable},
        m_maxTextures{maxTextures},
        m_initialLoadComplete{false}
{
}

auto TextureBufferResource::MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc
{
    Diligent::SamplerDesc samplerDesc{};
    samplerDesc.AddressU = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = Diligent::TEXTURE_ADDRESS_MODE::TEXTURE_ADDRESS_WRAP;

    return Diligent::ImmutableSamplerDesc{
        Diligent::SHADER_TYPE_PIXEL,
        variableName.data(),
        samplerDesc
    };
}

void TextureBufferResource::Load(std::span<const asset::TextureWithId> textures,
                                       Diligent::IDeviceContext& context,
                                       Diligent::IRenderDevice& device)
{
    if (!m_initialLoadComplete)
    {
        InitializeArray(context, device, m_variable, m_maxTextures);
        m_initialLoadComplete = true;
    }

    const auto textureCount = textures.size();
    if (textureCount == 0)
    {
        return;
    }

    if (textureCount + m_textures.size() > m_maxTextures)
    {
        throw NcError{"Max texture count exceeded"};
    }

    auto barriers = std::vector<Diligent::StateTransitionDesc>();
    barriers.reserve(textureCount);
    m_textures.reserve(m_textures.size() + textureCount);
    m_views.reserve(m_views.size() + textureCount);

    for (const auto& [texture, id, flags] : textures)
    {
        auto subResource = ToTextureSubResData(texture);
        auto texData = Diligent::TextureData{&subResource, 1, &context};
        auto desc = ToTextureDesc(texture, ToTextureFormat(flags));
        auto& textureHandle = m_textures.emplace_back();
        device.CreateTexture(desc, &texData, &textureHandle);
        if (!textureHandle)
        {
            throw NcError("Failed to create texture");
        }

        m_views.push_back(textureHandle->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE));
        barriers.emplace_back(
            textureHandle.RawPtr(),
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_SHADER_RESOURCE,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        );
    }

    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());
    auto offset = m_views.size() - textureCount;
    SetArrayRegion(m_variable, std::span<Diligent::IDeviceObject*>(m_views.data() + offset, textureCount), offset, textureCount);
}

void TextureBufferResource::Unload()
{
    m_textures.clear();
    m_textures.shrink_to_fit();
    m_views.clear();
    m_views.shrink_to_fit();
}
} // namespace nc::graphics
