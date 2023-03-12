#include "PerFrameRenderState.h"
#include "ecs/Registry.h"
#include "ecs/Transform.h"
#include "ecs/View.h"
#include "shader_resource/ShaderResourceService.h"


#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/PointLightSystem.h"
#include "system/WidgetSystem.h"

#include "optick/optick.h"


namespace nc::graphics
{
    PerFrameRenderState::PerFrameRenderState(Registry* registry,
                                             CameraFrontendState&& cameraState,
                                             EnvironmentFrontendState&& environmentState,
                                             ObjectFrontendState&& objectState,
                                             LightingFrontendState&& lightingState,
                                             WidgetSystemState&& widgetState,
                                             std::span<const nc::particle::EmitterState> particleEmitters)
        : camViewMatrix{ cameraState.view },
          projectionMatrix{ cameraState.projection },
          cameraPosition{ cameraState.position },
          meshes{ std::move(objectState.meshes) },
          #ifdef NC_EDITOR_ENABLED
          colliderDebugWidget{ widgetState.selectedCollider },
          #endif
          pointLightVPs{std::move(lightingState.viewProjections)},
          useSkybox{ environmentState.useSkybox },
          emitterStates{ particleEmitters },
          skyboxInstanceIndex{ objectState.skyboxInstanceIndex }
    {
        OPTICK_CATEGORY("PerFrameRenderState", Optick::Category::Rendering);
    }
}