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
                         MeshBuffer& meshBuffer,
                         Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                         Signal<const asset::MeshUpdateEventData&>& onMeshEvent)
            : m_assetDispatch{context, device, textureBuffer, meshBuffer, onTextureEvent, onMeshEvent},
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
