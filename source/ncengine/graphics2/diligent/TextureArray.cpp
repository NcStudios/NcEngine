#include "TextureArray.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "TextureLoader/interface/TextureLoader.h"


#include "TextureLoader/interface/Image.h"
#include "Common/interface/DataBlobImpl.hpp"

#include <vector>

namespace nc::graphics
{
void TextureArray::Load(std::span<const asset::TextureWithId> textures,
                        Diligent::IDeviceContext& context,
                        Diligent::IRenderDevice& device,
                        Diligent::IShaderResourceBinding& bindlessSRB)
{
    static bool run = false;
    if (run) return;
    run = true;

    const auto textureCount = textures.size();
    if (textureCount == 0)
    {
        return;
    }

    // auto loadInfo = Diligent::TextureLoadInfo{};
    // loadInfo.Format = Diligent::TEX_FORMAT_RGBA8_TYPELESS; // todo: no idea
    // loadInfo.MipLevels = 1;

    static auto texSRVs = std::vector<Diligent::IDeviceObject*>(textures.size());
    auto barriers = std::vector<Diligent::StateTransitionDesc>(textures.size());

    static auto apiTextures = std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>>(textures.size());
    static auto subresources = std::vector<Diligent::TextureSubResData>(textures.size());
    static auto srvs = std::vector<Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>>(textures.size());

    for (auto i = 0ull; i < textures.size(); ++i)
    {
        const auto& [texture, id, flags] = textures[i];

        auto texDesc = Diligent::TextureDesc{
            "",
            Diligent::RESOURCE_DIMENSION::RESOURCE_DIM_TEX_2D,
            texture.width,
            texture.height,
            1,
            // Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UINT
            // Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_SNORM
            Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM
            // Diligent::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM_SRGB
        };

        // don't know if this is right, but it caused it to bind
        texDesc.BindFlags = Diligent::BIND_FLAGS::BIND_SHADER_RESOURCE;
        // texDesc.BindFlags = Diligent::BIND_FLAGS::BIND_UNORDERED_ACCESS;

        auto& subResource = subresources[i] = (Diligent::TextureSubResData{
            texture.pixelData.data(),
            texture.width * 4u
        });

        auto texData = Diligent::TextureData{
            &subResource,
            1,
            &context
        };

        device.CreateTexture(texDesc, &texData, &apiTextures[i]);
        auto srv = apiTextures[i]->GetDefaultView(Diligent::TEXTURE_VIEW_SHADER_RESOURCE);
        texSRVs[i] = srv;
        barriers[i] = Diligent::StateTransitionDesc{
            apiTextures[i],
            Diligent::RESOURCE_STATE_UNKNOWN,
            Diligent::RESOURCE_STATE_SHADER_RESOURCE,
            Diligent::STATE_TRANSITION_FLAG_UPDATE_STATE
        };
    }

    context.TransitionResourceStates(static_cast<uint32_t>(barriers.size()), barriers.data());

    // if mbindless
    bindlessSRB.GetVariableByName(Diligent::SHADER_TYPE_PIXEL, "g_Texture")->SetArray(texSRVs.data(), 0, (uint32_t)texSRVs.size());
}


StubTextureResourceManager::StubTextureResourceManager(Diligent::IDeviceContext& context,
                                                       Diligent::IRenderDevice& device,
                                                       Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> bindlessSRB,
                                                       Signal<const asset::TextureUpdateEventData&>& onTextureEvent)
    : m_context{&context},
      m_device{&device},
      m_bindlessSRB{bindlessSRB},
      m_eventConnection{onTextureEvent.Connect(this, &StubTextureResourceManager::OnEvent)}
{
}

void StubTextureResourceManager::OnEvent(const asset::TextureUpdateEventData& event)
{
    switch (event.updateAction)
    {
        case asset::UpdateAction::Load:
        {
            m_textureArray.Load(event.data, *m_context, *m_device, *m_bindlessSRB);
            break;
        }
        // todo
    }
}
} // namespace nc::graphics
