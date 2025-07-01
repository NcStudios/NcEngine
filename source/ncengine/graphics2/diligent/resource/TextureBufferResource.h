#pragma once
#include "ncengine/asset/AssetData.h"
#include "ResourceTypes.h"

#include "ncutility/NcError.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc
{
namespace graphics
{
class TextureBufferResource
{
    public:
        explicit TextureBufferResource(Diligent::IShaderResourceVariable& variable, uint32_t maxTextures);
        static auto MakeSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;
        // static auto MakePointSamplerDesc(std::string_view variableName) -> Diligent::ImmutableSamplerDesc;

        template <typename T>
        void Load(std::span<const asset::TextureWithId<T>> textures,
                  Diligent::IDeviceContext& context,
                  Diligent::IRenderDevice& device);

        void Unload();

        auto GetShaderVariable() -> Diligent::IShaderResourceVariable&
        {
            return *m_variable;
        }

        auto GetMaxTextureCount() const -> size_t
        {
            return m_maxTextures;
        }

    private:
        std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;
        std::vector<Diligent::IDeviceObject*> m_views;
        Diligent::IShaderResourceVariable* m_variable;
        uint32_t m_maxTextures;
        bool m_initialLoadComplete;
};

template <typename T>
void TextureBufferResource::Load(std::span<const asset::TextureWithId<T>> textures,
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

    for (const auto& [texture, id] : textures)
    {
        auto mipSubResources = ToTextureSubResData<T>(texture);
        const auto mipLevels = static_cast<uint32_t>(mipSubResources.size());
        auto texData = Diligent::TextureData{mipSubResources.data(), mipLevels, &context};
        auto desc = ToTextureDesc<T>(texture);
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
} // namespace nc
} // namespace graphics
