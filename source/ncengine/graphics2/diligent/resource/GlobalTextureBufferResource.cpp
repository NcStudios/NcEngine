#include "GlobalTextureBufferResource.h"
#include "ncengine/asset/AssetData.h"

#include "TextureLoader.h"
#include "ncutility/NcError.h"

namespace
{
auto ToTextureFormat(nc::asset::asset_flags_type flags) -> Diligent::TEXTURE_FORMAT
{
    return flags & nc::asset::AssetFlags::TextureTypeNormalMap
        ? Diligent::TEX_FORMAT_RGBA8_UNORM
        : Diligent::TEX_FORMAT_RGBA8_UNORM_SRGB;
}

auto ToTextureDesc(const nc::asset::Texture& texture, Diligent::TEXTURE_FORMAT format) -> Diligent::TextureDesc
{
    /** @todo 750 Add mipmaps */
    auto texDesc = Diligent::TextureDesc{
        "",
        Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D,
        texture.width,
        texture.height,
        1,
        format
    };

    texDesc.BindFlags = Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;
    return texDesc;
}

auto ToTextureSubResData(const nc::asset::Texture& texture) -> Diligent::TextureSubResData
{
    return Diligent::TextureSubResData{texture.pixelData.data(), texture.width * 4u};
}
} // anonymous namespace

namespace nc::graphics
{
auto GlobalTextureBufferResource::MakeResourceDesc(std::string_view variableName, uint32_t maxTextures) -> Diligent::PipelineResourceDesc
{
    return Diligent::PipelineResourceDesc{
        Diligent::SHADER_TYPE::SHADER_TYPE_PIXEL,
        variableName.data(),
        maxTextures,
        Diligent::SHADER_RESOURCE_TYPE_TEXTURE_SRV,
        Diligent::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,
        Diligent::PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY
    };
}

auto GlobalTextureBufferResource::MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc
{
    return Diligent::ImmutableSamplerDesc{
        Diligent::SHADER_TYPE_PIXEL,
        variableName.data(),
        Diligent::SamplerDesc{}
    };
}

void GlobalTextureBufferResource::Load(std::span<const asset::TextureWithId> textures,
                                       Diligent::IDeviceContext& context,
                                       Diligent::IRenderDevice& device)
{
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
    SetArrayRegion(m_views.size() - textureCount, textureCount);
}

void GlobalTextureBufferResource::Unload()
{
    m_textures.clear();
    m_textures.shrink_to_fit();
    m_views.clear();
    m_views.shrink_to_fit();
}

void GlobalTextureBufferResource::SetArrayRegion(size_t offset, size_t count)
{
    m_variable->SetArray(
        m_views.data() + offset,
        static_cast<uint32_t>(offset),
        static_cast<uint32_t>(count),
        Diligent::SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE
    );
}
} // namespace nc::graphics
