#include "LightSubsystem.h"
#include "CascadedShadowMap.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Light.h"

#include <ranges>

namespace directionallight
{
const auto g_sceneCenter = DirectX::g_XMZero;
auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix, DirectX::FXMMATRIX projectionMatrix, float sceneExtentY) -> DirectX::XMMATRIX
{
    DirectX::XMMATRIX rotationMatrix = transformMatrix;
    rotationMatrix.r[3] = DirectX::g_XMIdentityR3; // Directional lights have no position component

    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, rotationMatrix);
    auto eye = DirectX::XMVectorSubtract(g_sceneCenter, DirectX::XMVectorScale(look, sceneExtentY * 0.5f));
    return DirectX::XMMatrixLookAtRH(eye, DirectX::XMVectorZero(), DirectX::g_XMIdentityR1) * projectionMatrix;
}
} // namespace directionallight

namespace spotlight2
{
auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix, float outerAngle, float farClip) -> DirectX::XMMATRIX
{
    // Compute the effective outer angle in radians
    float lightFieldOfView = 2 * outerAngle;

    // Create a perspective projection matrix that matches the cone
    auto projectionMatrix = DirectX::XMMatrixPerspectiveRH(lightFieldOfView, 2.0f, 1.0f, farClip);
    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * projectionMatrix;
}
}
namespace nc::graphics
{
auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs, const CameraRenderState& cameraState) -> LightRenderState
{
    m_lightData.clear();
    m_lightMatrixData.clear();
    m_cascadeData.clear();
    auto lightMatrixIndex = 0u;

    { // Directional Lights
        const auto& pool = ecs.GetPool<DirectionalLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            const auto lightMatrixStartIndex = static_cast<uint32_t>(m_lightMatrixData.size());
            const auto cascadeStartIndex = static_cast<uint32_t>(m_cascadeData.size());

            // Get light direction from transform
            const auto direction = DirectX::XMVector3TransformNormal(
                DirectX::g_XMIdentityR2,
                transform.TransformationMatrix()
            );

            if (light.castsShadows && m_cascadedShadowMap)
            {
                // Update CSM with current camera
                auto invViewProj = DirectX::XMMatrixInverse(nullptr, cameraState.viewProjection);
                m_cascadedShadowMap->Update(
                    invViewProj,
                    direction,
                    cameraState.nearClip,
                    cameraState.farClip
                );

                // Copy cascade data
                for (const auto& cascade : m_cascadedShadowMap->GetCascadeData())
                {
                    m_cascadeData.push_back(cascade);
                    m_lightMatrixData.push_back(LightMatrixData{
                        .viewProjection = cascade.viewProjection
                    });
                }
                lightMatrixIndex++;
            }
            else if (light.castsShadows)
            {
                // Legacy single shadow map fallback
                m_lightMatrixData.push_back(LightMatrixData{
                    .viewProjection = directionallight::CalculateLightViewProjectionMatrix(
                        transform.TransformationMatrix(),
                        m_directionalLightProjection,
                        m_sceneExtentY
                    )
                });
                lightMatrixIndex++;
            }

            m_lightData.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Forward(),
                light.castsShadows,
                lightMatrixStartIndex,
                cascadeStartIndex,
                m_cascadedShadowMap ? m_cascadedShadowMap->GetCascadeCount() : 0u
            );
        }
    }

    { // Point Lights
        const auto& pool = ecs.GetPool<PointLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_lightData.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                light.castsShadows,
                light.radius,
                lightMatrixIndex
            );

            if (light.castsShadows)
            {
                // Positive X
                {
                    const auto look = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
                // Negative X
                {
                    const auto look = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
                // Positive Y
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(),  DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
                    // Negative Y
                {
                    const auto look = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(),  DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
 
                // Positive Z
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
                // Negative Z
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, m_pointLightProjection)});
                    lightMatrixIndex++;
                }
            }
        }
    }

    { // Spot Lights
        const auto& pool = ecs.GetPool<SpotLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            float outerAngle = cos(std::max(light.outerAngle, 0.0001f)) * (1 - light.radius * 0.01f);
            auto& transform = ecs.Get<Transform>(entity);

            m_lightData.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                cos(std::max(light.innerAngle, 0.0001f)) * (1 - light.radius * 0.01f),
                transform.Forward(),
                outerAngle,
                light.radius,
                light.castsShadows,
                lightMatrixIndex
            );

            if (light.castsShadows)
            {
                m_lightMatrixData.push_back(LightMatrixData{.viewProjection = spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix(), (1-outerAngle) * 1.75f, light.radius)});
                lightMatrixIndex++;
            }
        }
    }
    return LightRenderState{m_lightData, m_lightMatrixData, m_cascadeData};
}

void LightSubsystem::OnBeforeSceneLoad(const nc::Vector3& extents)
{
    m_sceneExtentY = extents.y;
    m_directionalLightProjection = DirectX::XMMatrixOrthographicRH(extents.x, extents.y, 1.0f, extents.z);
    m_pointLightProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 1.0f, extents.z); // LH is needed for cubemap projection
    // Spot light projection is computed based on light properties

    // Initialize CSM from graphics config
    const auto& graphicsSettings = config::GetGraphicsSettings();
    CascadeShadowConfig csmConfig{
          .cascadeCount = graphicsSettings.csmCascadeCount,
          .shadowDistance = std::min(graphicsSettings.csmShadowDistance, extents.z),
          .splitLambda = graphicsSettings.csmSplitLambda,
          .blendRegion = 0.3f, // Keep in sync with shader
          .shadowMapResolution = static_cast<float>(graphicsSettings.shadowMapResolution),
          .stabilizeCascades = graphicsSettings.csmStabilize
    };
    m_cascadedShadowMap = std::make_unique<CascadedShadowMap>(csmConfig);
}

void LightSubsystem::SetCascadeConfig(const CascadeShadowConfig& config)
{
    if (m_cascadedShadowMap)
    {
        m_cascadedShadowMap->SetConfig(config);
    }
}

const CascadeShadowConfig& LightSubsystem::GetCascadeConfig() const
{
    static CascadeShadowConfig defaultConfig{};
    return m_cascadedShadowMap ? m_cascadedShadowMap->GetConfig() : defaultConfig;
}
} // namespace nc::graphics
