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
auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix, float outerAngleRadians, float farClip) -> DirectX::XMMATRIX
{
    // The outer angle is the half-angle of the cone, so FOV is 2x that
    const float lightFieldOfView = 2.0f * outerAngleRadians;
    constexpr float nearClip = 0.1f;

    // Use FOV-based perspective projection with 1:1 aspect ratio (square shadow map)
    auto projectionMatrix = DirectX::XMMatrixPerspectiveFovRH(lightFieldOfView, 1.0f, nearClip, farClip);
    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], DirectX::XMVectorAdd(transformMatrix.r[3], look), DirectX::g_XMNegIdentityR1) * projectionMatrix;
}
}
namespace nc::graphics
{
auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs, const CameraRenderState& cameraState) -> LightRenderState
{
    m_lightData.clear();
    m_lightMatrixData.clear();
    m_cascadeData.clear();

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
            const auto lightMatrixStartIndex = static_cast<uint32_t>(m_lightMatrixData.size());

            m_lightData.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                light.castsShadows,
                light.radius,
                lightMatrixStartIndex
            );

            if (light.castsShadows)
            {
                // Create per-light projection constrained to light's radius
                constexpr float nearClip = 0.1f;
                const auto pointLightProjection = DirectX::XMMatrixPerspectiveFovLH(
                    DirectX::XM_PIDIV2, 1.0f, nearClip, light.radius
                );

                // Positive X
                {
                    const auto look = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
                // Negative X
                {
                    const auto look = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
                // Positive Y
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(),  DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
                // Negative Y
                {
                    const auto look = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(),  DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
                // Positive Z
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
                // Negative Z
                {
                    const auto look = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
                    auto viewMatrix = DirectX::XMMatrixLookAtLH(transform.PositionXM(), DirectX::XMVectorAdd(transform.PositionXM(),look), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
                    m_lightMatrixData.push_back(LightMatrixData{.viewProjection = DirectX::XMMatrixMultiply(viewMatrix, pointLightProjection)});
                }
            }
        }
    }

    { // Spot Lights
        const auto& pool = ecs.GetPool<SpotLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            // Compute cosine-based angles for shader attenuation
            float outerAngleCos = cos(std::max(light.outerAngle, 0.0001f)) * (1 - light.radius * 0.01f);
            auto& transform = ecs.Get<Transform>(entity);
            const auto lightMatrixStartIndex = static_cast<uint32_t>(m_lightMatrixData.size());

            m_lightData.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                cos(std::max(light.innerAngle, 0.0001f)) * (1 - light.radius * 0.01f),
                transform.Forward(),
                outerAngleCos,
                light.radius,
                light.castsShadows,
                lightMatrixStartIndex
            );

            if (light.castsShadows)
            {
                // Pass the actual outer angle (radians) for FOV-based projection
                m_lightMatrixData.push_back(LightMatrixData{.viewProjection = spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix(), light.outerAngle, light.radius)});
            }
        }
    }
    return LightRenderState{m_lightData, m_lightMatrixData, m_cascadeData};
}

void LightSubsystem::OnBeforeSceneLoad(const nc::Vector3& extents)
{
    m_sceneExtentY = extents.y;
    m_directionalLightProjection = DirectX::XMMatrixOrthographicRH(extents.x, extents.y, 1.0f, extents.z);
    // Point light and spot light projections are computed per-light based on their radius

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
