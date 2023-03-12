#include "PerFrameRenderState.h"
#include "ecs/Registry.h"
#include "ecs/Transform.h"
#include "ecs/View.h"
#include "shader_resource/ShaderResourceService.h"


#include "CameraState.h"
#include "Environment.h"
#include "ObjectFrontend.h"
#include "PointLightSystem.h"

#include "optick/optick.h"


namespace nc::graphics
{
    PerFrameRenderState::PerFrameRenderState(Registry* registry,
                                            const CameraFrontendState& cameraState,
                                            const EnvironmentFrontendState& environmentState,
                                            const ObjectFrontendState& objectState,
                                            const LightingFrontendState& lightingState,
                                             std::span<const nc::particle::EmitterState> particleEmitters)
        : camViewMatrix{ cameraState.view },
          projectionMatrix{ cameraState.projection },
          cameraPosition{ cameraState.position },
          meshes{ objectState.meshes },
          #ifdef NC_EDITOR_ENABLED
          colliderDebugWidget{ std::nullopt },
          #endif
          pointLightVPs{lightingState.viewProjections},
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