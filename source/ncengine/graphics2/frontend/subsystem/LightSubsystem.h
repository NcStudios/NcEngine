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
        LightSubsystem(uint32_t shadowMapResX)
            : m_shadowMapResX{shadowMapResX} {}
        auto BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState;
        void OnBeforeSceneLoad(const nc::Vector3& extents);

    private:
        std::vector<LightData> m_lightData;
        std::vector<LightMatrixData> m_lightMatrixData;
        DirectX::XMMATRIX m_directionalLightProjection;
        DirectX::XMMATRIX m_pointLightProjection;
        float m_sceneExtentY;
        float m_nearZ = 1.0f;
        float m_farZ = 40.0f;
        uint32_t m_shadowMapResX = 512;
};
} // namespace graphics
} // namespace nc
