#pragma once

#include "diligent/resource/GlobalTextureBufferResource.h"

#include "ncengine/asset/AssetData.h"
#include "ncengine/utility/Signal.h"

#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class AssetDispatch
{
    public:
        AssetDispatch(Diligent::IDeviceContext& context,
                      Diligent::IRenderDevice& device,
                      GlobalTextureBufferResource& textureBuffer,
                      Signal<const asset::TextureUpdateEventData&>& onTextureEvent)
            : m_context{&context},
              m_device{&device},
              m_textureBuffer{&textureBuffer},
              m_textureConnection{onTextureEvent.Connect(this, &AssetDispatch::OnTextureEvent)}
        {
        }

    private:
        Diligent::IDeviceContext* m_context;
        Diligent::IRenderDevice* m_device;
        GlobalTextureBufferResource* m_textureBuffer;
        Connection m_textureConnection;

        void OnTextureEvent(const asset::TextureUpdateEventData& event)
        {
            switch (event.updateAction)
            {
                case asset::UpdateAction::Load:
                {
                    m_textureBuffer->Load(event.data, *m_context, *m_device);
                    break;
                }
                case asset::UpdateAction::UnloadAll:
                {
                    m_textureBuffer->Unload();
                    break;
                }
                case asset::UpdateAction::Unload:
                {
                    throw "unhandled action";
                }
            }
        }
};
} // namespace nc::graphics
