#include "TextureBufferResource.h"

#include "TextureLoader.h"

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

void TextureBufferResource::Unload()
{
    m_textures.clear();
    m_textures.shrink_to_fit();
    m_views.clear();
    m_views.shrink_to_fit();
}
} // namespace nc::graphics
