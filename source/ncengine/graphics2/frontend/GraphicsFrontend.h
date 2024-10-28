#pragma once

#include "subsystem/AssetDispatch.h"
#include "subsystem/CameraSubsystem.h"
#include "subsystem/UISubsystem.h"

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
                         Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                         ecs::Ecs world,
                         ModuleProvider modules,
                         SystemEvents& events)
            : m_assetDispatch{context, device, textureBuffer, onTextureEvent},
              m_uiSystem{world, modules, events},
              m_cameraSystem{}
        {
        }

        auto BuildRenderState(ecs::Ecs world) -> FrontendRenderState;

        void Clear() noexcept
        {
            m_cameraSystem.Clear();
        }

        auto GetCameraSubsystem()   ->       CameraSubsystem& { return m_cameraSystem; }
        auto GetUISubsystem()       ->       UISubsystem&     { return m_uiSystem;     }
        auto GetUISubsystem() const -> const UISubsystem&     { return m_uiSystem;     }

    private:
        AssetDispatch m_assetDispatch;
        UISubsystem m_uiSystem;
        CameraSubsystem m_cameraSystem;
};
} // namespace nc::graphics
