#pragma once

#include "subsystem/animation/SkeletalAnimationSubsystem.h"
#include "subsystem/AssetDispatch.h"
#include "subsystem/CameraSubsystem.h"
#include "subsystem/EnvironmentSubsystem.h"
#include "subsystem/LightSubsystem.h"
#include "subsystem/MaterialRegistry.h"
#include "subsystem/MeshSubsystem.h"
#include "subsystem/particle/ParticleSubsystem.h"
#include "subsystem/PostProcessSubsystem.h"
#include "subsystem/UISubsystem.h"
#include "subsystem/WireframeRendererSubsystem.h"
#include "subsystem/animation/SkeletalAnimationSubsystem.h"

#include "ncengine/ecs/EcsFwd.h"

namespace nc::graphics
{
struct FrontendRenderState;

class GraphicsFrontend
{
    public:
        GraphicsFrontend(Diligent::IDeviceContext& context,
                         Diligent::IRenderDevice& device,
                         CubeMapBufferResource& cubeMapBuffer,
                         TextureBufferResource& textureBuffer,
                         MeshBuffer& meshBuffer,
                         ecs::Ecs world,
                         ModuleProvider modules,
                         SystemEvents& events,
                         uint32_t maxEntities,
                         uint32_t maxRenderers,
                         uint32_t maxBones,
                         uint32_t maxParticles,
                         uint32_t initialBatchSize,
                         Signal<const asset::CubeMapUpdateEventData&>& onCubeMapEvent,
                         Signal<const asset::TextureUpdateEventData&>& onTextureEvent,
                         Signal<const asset::MeshUpdateEventData&>& onMeshEvent,
                         Signal<const asset::SkeletalAnimationUpdateEventData&>& onAnimationEvent,
                         Signal<const asset::BoneUpdateEventData&>& onBoneEvent)
            : m_animationSystem{maxBones},
              m_assetDispatch{
                context,
                device,
                cubeMapBuffer,
                textureBuffer,
                meshBuffer,
                m_animationSystem.GetStorage(),
                onCubeMapEvent,
                onTextureEvent,
                onMeshEvent,
                onAnimationEvent,
                onBoneEvent
              },
              m_materialRegistry{maxRenderers},
              m_uiSystem{world, modules, events},
              m_cameraSystem{},
              m_environmentSystem{},
              m_meshSystem{
                m_animationSystem,
                events,
                maxEntities,
                maxRenderers,
                initialBatchSize
              },
              m_particleSystem{
                world,
                maxParticles
              }
        {
        }

        auto BuildRenderState(ecs::Ecs world) -> FrontendRenderState;

        void OnBeforeSceneLoad()
        {
            m_animationSystem.OnBeforeSceneLoad();
            m_meshSystem.OnBeforeSceneLoad();
        }

        void Clear() noexcept
        {
            m_cameraSystem.Clear();
            m_particleSystem.Clear();
        }

        auto GetCameraSubsystem()                  ->       CameraSubsystem&            { return m_cameraSystem;      }
        auto GetEnvironmentSubsystem()       const -> const EnvironmentSubsystem&       { return m_environmentSystem; }
        auto GetEnvironmentSubsystem()             ->       EnvironmentSubsystem&       { return m_environmentSystem; }
        auto GetMeshSubsystem()                    ->       MeshSubsystem&              { return m_meshSystem;        }
        auto GetMaterialRegistry()                 ->       MaterialRegistry&           { return m_materialRegistry;  }
        auto GetPostProcessSubsystem()             ->       PostProcessSubsystem&       { return m_postProcessSystem; }
        auto GetPostProcessSubsystem()       const -> const PostProcessSubsystem&       { return m_postProcessSystem; }
        auto GetSkeletalAnimationSubsystem()       ->       SkeletalAnimationSubsystem& { return m_animationSystem;   }
        auto GetParticleSubsystem()                ->       ParticleSubsystem&          { return m_particleSystem;    }
        auto GetUISubsystem()                      ->       UISubsystem&                { return m_uiSystem;          }
        auto GetUISubsystem()                const -> const UISubsystem&                { return m_uiSystem;          }
        auto GetWireframeSubsystem()               ->       WireframeRendererSubsystem& { return m_wireframeSystem;   }

    private:
        SkeletalAnimationSubsystem m_animationSystem;
        AssetDispatch m_assetDispatch;
        MaterialRegistry m_materialRegistry;
        UISubsystem m_uiSystem;
        CameraSubsystem m_cameraSystem;
        EnvironmentSubsystem m_environmentSystem;
        MeshSubsystem m_meshSystem;
        ParticleSubsystem m_particleSystem;
        LightSubsystem m_lightSubsystem;
        PostProcessSubsystem m_postProcessSystem;
        WireframeRendererSubsystem m_wireframeSystem;
};
} // namespace nc::graphics
