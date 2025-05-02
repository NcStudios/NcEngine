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
        void OnBeforeSceneLoad(const nc::Vector3& extents);

    private:
        std::vector<LightData> m_lightData;
        std::vector<LightMatrixData> m_lightMatrixData;
        DirectX::XMMATRIX m_directionalLightProjection;
        DirectX::XMMATRIX m_pointLightProjection;
        float m_sceneExtentY;
};
} // namespace graphics
} // namespace nc
