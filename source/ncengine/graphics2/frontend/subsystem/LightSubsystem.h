#pragma once

#include "LightRenderState.h"

#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/EcsFwd.h"


#include <vector>

namespace nc
{
class Transform;
namespace graphics
{
struct DirectionalLight;
struct PointLight;
struct SpotLight;

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
