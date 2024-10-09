#pragma once

#include "ncengine/asset/AssetData.h"
#include "ncengine/utility/Signal.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/Texture.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/ShaderResourceBinding.h"

namespace Diligent
{
struct IDeviceContext;
struct IRenderDevice;
} // namespace Diligent

namespace nc::graphics
{
class TextureArray
{
    public:
        void Load(std::span<const asset::TextureWithId> textures,
                  Diligent::IDeviceContext& context,
                  Diligent::IRenderDevice& device,
                  Diligent::IShaderResourceBinding& bindlessSRB);

    private:
        Diligent::RefCntAutoPtr<Diligent::ITexture> m_textureArray;
        Diligent::ITextureView* m_textureView = nullptr;
};

class StubTextureResourceManager
{
    public:
        StubTextureResourceManager(Diligent::IDeviceContext& context,
                                   Diligent::IRenderDevice& device,
                                   Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> bindlessSRB,
                                   Signal<const asset::TextureUpdateEventData&>& onTextureEvent);

    private:
        Diligent::IDeviceContext* m_context;
        Diligent::IRenderDevice* m_device;
        Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_bindlessSRB;
        TextureArray m_textureArray;
        Connection m_eventConnection;

        void OnEvent(const asset::TextureUpdateEventData& event);
};
} // namespace nc::graphics
