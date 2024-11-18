#pragma once

#include "LightRenderState.h"
#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/DirectionalLight.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SpotLight.h"

#include <vector>

namespace nc
{
namespace graphics
{
class LightSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState;

    private:
        std::vector<DirectionalLightData> m_directionalLights;
        std::vector<PointLightData> m_pointLights;
        std::vector<SpotLightData> m_spotLights;
};
} // namespace graphics
} // namespace nc
