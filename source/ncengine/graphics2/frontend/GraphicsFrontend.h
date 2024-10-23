#pragma once

#include "subsystem/AssetDispatch.h"
#include "subsystem/CameraSubsystem.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc::graphics
{
struct FrontendRenderState;

class GraphicsFrontend
{
    public:
        GraphicsFrontend(Diligent::IDeviceContext& context,
                         Diligent::IRenderDevice& device,
                         GlobalTextureBufferResource& textureBuffer,
                         Signal<const asset::TextureUpdateEventData&>& onTextureEvent)
            : m_assetDispatch{context, device, textureBuffer, onTextureEvent},
              m_cameraSystem{}
        {
        }

        auto BuildRenderState(ecs::Ecs world) -> FrontendRenderState;

        void Clear() noexcept
        {
            m_cameraSystem.Clear();
        }

        auto GetCameraSubsystem() -> CameraSubsystem&
        {
            return m_cameraSystem;
        }

    private:
        AssetDispatch m_assetDispatch;
        CameraSubsystem m_cameraSystem;
};
} // namespace nc::graphics
