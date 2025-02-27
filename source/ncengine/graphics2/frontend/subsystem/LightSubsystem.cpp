#include "LightSubsystem.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Light.h"

#include <ranges>

namespace pointlight2
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(60.0f);
constexpr float g_nearClip = 0.25f;
constexpr float g_farClip = 96.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace pointlight2

namespace spotlight2
{
constexpr float g_nearClip = 0.3f;
constexpr float g_farClip = 200.f;

auto CalculateLightViewProjectionMatrix(float arcRadians, DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * DirectX::XMMatrixPerspectiveRH(arcRadians, 1.0f, g_nearClip, g_farClip);
}
} // namespace spotlight2

namespace nc::graphics
{
auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Transform> ecs) -> LightRenderState
{
    m_data.clear();

    { // Directional Lights
        const auto& pool = ecs.GetPool<DirectionalLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(light.diffuseColor, light.specularColor, light.intensity, transform.Forward());
        }
    }

    { // Point Lights
        const auto& pool = ecs.GetPool<PointLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                0, /** @todo, come up with shadow decisioning (which lights cast shadows) */
                light.radius,
                pointlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix())
            );
        }
    }

    { // Spot Lights
        const auto& pool = ecs.GetPool<SpotLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            float innerAngle = cos(std::max<float>(light.innerAngle, 0.0001f));
            float outerAngle = cos(std::max<float>(light.outerAngle, 0.0001f));
            float radius = std::max<float>(light.radius, 0.0001f);


            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                innerAngle,
                transform.Forward(),
                outerAngle,
                radius,
                1, /** @todo, come up with shadow decisioning (which lights cast shadows) */
                spotlight2::CalculateLightViewProjectionMatrix(std::max<float>(std::max<float>(innerAngle, innerAngle-outerAngle) * (radius*0.025f), 0.0001f), transform.TransformationMatrix())
            );
        }
    }

    return LightRenderState{m_data};
}
} // namespace nc::graphics
