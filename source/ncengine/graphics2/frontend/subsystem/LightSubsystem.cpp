#include "LightSubsystem.h"

#include <ranges>

namespace pointlight2
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(60.0f);
constexpr float g_nearClip = 0.25f;
constexpr float g_farClip = 96.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace pointlight2

namespace spotlight2
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(45.0f);
constexpr float g_nearClip = 1.0f;
constexpr float g_farClip = 100.f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace spotlight2

namespace nc::graphics
{
auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState
{
    { // Directional Lights
        const auto& lightPool = ecs.GetPool<DirectionalLight>();
        const auto entities = lightPool.GetEntityPool();
        m_directionalLights.clear();
        m_directionalLights.reserve(entities.size());

        for (auto [i, entity] : std::views::enumerate(entities))
        {
            auto& light = ecs.Get<DirectionalLight>(entity);
            auto& transform = ecs.Get<Transform>(entity);

            m_directionalLights.emplace_back(light.color, transform.Forward());
        }
    }

    { // Point Lights
        const auto& lightPool = ecs.GetPool<PointLight>();
        const auto entities = lightPool.GetEntityPool();
        m_pointLights.clear();
        m_pointLights.reserve(entities.size());

        for (auto [i, entity] : std::views::enumerate(entities))
        {
            auto& light = ecs.Get<PointLight>(entity);
            auto& transform = ecs.Get<Transform>(entity);

            m_pointLights.emplace_back(pointlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix()),
                                       transform.Position(),
                                       0, /** @todo, come up with shadow decisioning (which lights cast shadows) */
                                       light.diffuseColor,
                                       light.radius);
        }
    }

    { // Spot Lights
        const auto& lightPool = ecs.GetPool<SpotLight>();
        const auto entities = lightPool.GetEntityPool();
        m_spotLights.clear();
        m_spotLights.reserve(entities.size());

        for (auto [i, entity] : std::views::enumerate(entities))
        {
            auto& light = ecs.Get<SpotLight>(entity);
            auto& transform = ecs.Get<Transform>(entity);

            m_spotLights.emplace_back(spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix()),
                                      transform.Position(),
                                      0, /** @todo, come up with shadow decisioning (which lights cast shadows) */
                                      light.color,
                                      light.innerAngle,
                                      transform.Forward(),
                                      light.outerAngle,
                                      light.radius);
        }
    }

    return LightRenderState
    {
        m_directionalLights,
        m_pointLights,
        m_spotLights
    };
}
} // namespace nc::graphics
