#include "PerFrameRenderState.h"
#include "ecs/Registry.h"
#include "ecs/Transform.h"
#include "ecs/View.h"
#include "shader_resource/ShaderResourceService.h"


#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/PointLightSystem.h"

#include "optick/optick.h"


namespace nc::graphics
{
    PerFrameRenderState::PerFrameRenderState(Registry* registry,
                                             CameraFrontendState&& cameraState,
                                             EnvironmentFrontendState&& environmentState,
                                             ObjectFrontendState&& objectState,
                                             LightingFrontendState&& lightingState,
                                             std::span<const nc::particle::EmitterState> particleEmitters)
        : camViewMatrix{ cameraState.view },
          projectionMatrix{ cameraState.projection },
          cameraPosition{ cameraState.position },
          meshes{ std::move(objectState.meshes) },
          #ifdef NC_EDITOR_ENABLED
          colliderDebugWidget{ std::nullopt },
          #endif
          pointLightVPs{std::move(lightingState.viewProjections)},
          useSkybox{ environmentState.useSkybox },
          emitterStates{ particleEmitters },
          skyboxInstanceIndex{ objectState.skyboxInstanceIndex }
    {
        OPTICK_CATEGORY("PerFrameRenderState", Optick::Category::Rendering);

        #ifdef NC_EDITOR_ENABLED
        auto colliderIsSelected = false;
        for (auto& collider : View<physics::Collider>{ registry })
        {
            if (collider.GetEditorSelection())
            {
                colliderDebugWidget = collider.GetDebugWidget();
                colliderIsSelected = true;
            }
        }

        if (!colliderIsSelected) colliderDebugWidget = std::nullopt;
        #endif
    }

}