#pragma once

#include "LightRenderState.h"
#include "CascadedShadowMap.h"
#include "graphics2/ShaderTypes.h"
#include "CameraRenderState.h"

#include "ncengine/ecs/EcsFwd.h"

#include <vector>
#include <memory>

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
        auto BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs, const CameraRenderState& cameraState) -> LightRenderState;
        void OnBeforeSceneLoad(const nc::Vector3& extents);

        void SetCascadeConfig(const CascadeShadowConfig& config);
        [[nodiscard]] auto GetCascadeConfig() const -> const CascadeShadowConfig&;

    private:
        std::vector<LightData> m_lightData;
        std::vector<LightMatrixData> m_lightMatrixData;
        std::vector<CascadeData> m_cascadeData;

        DirectX::XMMATRIX m_directionalLightProjection;
        DirectX::XMMATRIX m_pointLightProjection;
        float m_sceneExtentY = 1.0f;
        std::unique_ptr<CascadedShadowMap> m_cascadedShadowMap;
};
} // namespace graphics
} // namespace nc
