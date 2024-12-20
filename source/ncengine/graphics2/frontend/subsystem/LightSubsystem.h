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

struct Light;

class LightSubsystem
{
    public:
        auto BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Light, Transform> ecs) -> LightRenderState;

    private:
        std::vector<LightData> m_data; // todo: HostStructuredBuffer + ignore statics
};
} // namespace graphics
} // namespace nc
