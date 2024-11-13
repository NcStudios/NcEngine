#pragma once

#include "ncengine/graphics/GraphicsUtility.h"

#include "subsystem/AssetDispatch.h"
#include "subsystem/CameraSubsystem.h"
#include "subsystem/MaterialRegistry.h"
#include "subsystem/MeshRendererSubsystem.h"
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
                         GlobalMeshBuffer& meshBuffer,
                         ecs::Ecs world,
                         ModuleProvider modules,
                         SystemEvents& events,
                         uint32_t maxRenderers,
                         Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                         Signal<const asset::MeshUpdateEventData&>& onMeshEvent)
            : m_assetDispatch{context, device, textureBuffer, meshBuffer, onTextureEvent, onMeshEvent},
              m_materialRegistry{maxRenderers},
              m_uiSystem{world, modules, events},
              m_cameraSystem{},
              m_meshRendererSystem{GetImplementedMaterialPassFlags()}
        {
        }

        auto BuildRenderState(ecs::Ecs world) -> FrontendRenderState;

        void Clear() noexcept
        {
            m_cameraSystem.Clear();
        }

        auto GetCameraSubsystem()       ->       CameraSubsystem&       { return m_cameraSystem;       }
        auto GetMeshRendererSubsystem() ->       MeshRendererSubsystem& { return m_meshRendererSystem; }
        auto GetMaterialRegistry()      ->       MaterialRegistry&      { return m_materialRegistry;   }
        auto GetUISubsystem()           ->       UISubsystem&           { return m_uiSystem;           }
        auto GetUISubsystem() const     -> const UISubsystem&           { return m_uiSystem;           }

    private:
        AssetDispatch m_assetDispatch;
        MaterialRegistry m_materialRegistry;
        UISubsystem m_uiSystem;
        CameraSubsystem m_cameraSystem;
        MeshRendererSubsystem m_meshRendererSystem;
};
} // namespace nc::graphics
