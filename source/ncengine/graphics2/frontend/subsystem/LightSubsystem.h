#pragma once

#include "LightRenderState.h"

#include "graphics2/ShaderTypes.h"

#include "ncengine/ecs/EcsFwd.h"

#include <vector>

namespace nc
{
struct DirectionalLight;
struct PointLight;
struct SpotLight;
class Transform;

namespace graphics
{
class LightSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState;

    private:
        std::vector<LightData> m_data;
};
} // namespace graphics
} // namespace nc
