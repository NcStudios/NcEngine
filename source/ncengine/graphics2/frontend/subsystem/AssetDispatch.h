#pragma once

#include "graphics2/diligent/resource/GlobalMeshBuffer.h"
#include "graphics2/diligent/resource/GlobalTextureBufferResource.h"

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
                      GlobalMeshBuffer& meshBuffer,
                      Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                      Signal<const asset::MeshUpdateEventData&>& onMeshEvent)
            : m_context{&context},
              m_device{&device},
              m_textureBuffer{&textureBuffer},
              m_meshBuffer{&meshBuffer},
              m_textureConnection{onTextureEvent.Connect(this, &AssetDispatch::OnTextureEvent)},
              m_meshConnection{onMeshEvent.Connect(this, &AssetDispatch::OnMeshEvent)}
        {
        }

    private:
        Diligent::IDeviceContext* m_context;
        Diligent::IRenderDevice* m_device;
        GlobalTextureBufferResource* m_textureBuffer;
        GlobalMeshBuffer* m_meshBuffer;
        Connection m_textureConnection;
        Connection m_meshConnection;

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

        void OnMeshEvent(const asset::MeshUpdateEventData& event)
        {
            m_meshBuffer->Load(event.vertices, event.indices, *m_context, *m_device);
        }
};
} // namespace nc::graphics
