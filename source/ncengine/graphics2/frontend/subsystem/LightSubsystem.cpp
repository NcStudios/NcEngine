#include "LightSubsystem.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/Light.h"

#include <ranges>

namespace directionallight
{
constexpr float g_nearClip = 1.0f;
constexpr float g_farClip = 150.0f;
const auto g_sceneCenter = DirectX::XMVectorZero();
const auto g_lightProjectionMatrix = DirectX::XMMatrixOrthographicRH(150.0f, 150.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    DirectX::XMMATRIX rotationMatrix = transformMatrix;
    rotationMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Directional lights have no position component

    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, rotationMatrix);
    auto eye = DirectX::XMVectorSubtract(g_sceneCenter, DirectX::XMVectorScale(look, 100.0));
    return DirectX::XMMatrixLookAtRH(eye, DirectX::XMVectorZero(), DirectX::g_XMIdentityR1) * g_lightProjectionMatrix;
}
} // namespace directionallight

namespace pointlight2
{
constexpr float g_lightFieldOfView = nc::DegreesToRadians(60.0f);
constexpr float g_nearClip = 0.25f;
constexpr float g_farClip = 96.0f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace pointlight2

namespace spotlight2
{
constexpr float g_nearClip = 0.5f;

auto CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix, float outerAngle, float farClip) -> DirectX::XMMATRIX
{
    // Compute the effective outer angle in radians
    float lightFieldOfView = 2 * outerAngle;

    // Create a perspective projection matrix that matches the cone
    auto lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(lightFieldOfView, 2.0f, g_nearClip, farClip);
    const auto look = DirectX::XMVector3TransformNormal(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * lightProjectionMatrix;
}
}
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
            auto shadowMapMatrix = DirectX::XMMatrixIdentity();

            if (light.castsShadows == 1)
            {
                shadowMapMatrix = directionallight::CalculateLightViewProjectionMatrix(transform.TransformationMatrix());
            }

            m_data.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Forward(),
                light.castsShadows,
                shadowMapMatrix
            );
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
            float outerAngle = cos(std::max(light.outerAngle, 0.0001f)) * (1 - light.radius * 0.01f);
            auto& transform = ecs.Get<Transform>(entity);
            auto shadowMapMatrix = DirectX::XMMatrixIdentity();

            if (light.castsShadows == 1)
            {
                shadowMapMatrix = spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix(), (1-outerAngle)*1.75f, light.radius);
            }

            m_data.emplace_back(
                light.diffuseColor,
                light.specularColor,
                light.intensity,
                transform.Position(),
                cos(std::max(light.innerAngle, 0.0001f)) * (1 - light.radius * 0.01f),
                transform.Forward(),
                outerAngle,
                light.radius,
                light.castsShadows,
                shadowMapMatrix
            );
        }
    }
    return LightRenderState{m_data};
}
} // namespace nc::graphics
