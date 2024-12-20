#include "LightSubsystem.h"

#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/Transform.h"
#include "ncengine/graphics/DirectionalLight.h"
#include "ncengine/graphics/PointLight.h"
#include "ncengine/graphics/SpotLight.h"
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
constexpr float g_lightFieldOfView = nc::DegreesToRadians(45.0f);
constexpr float g_nearClip = 1.0f;
constexpr float g_farClip = 100.f;
const auto g_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(g_lightFieldOfView, 1.0f, g_nearClip, g_farClip);

auto CalculateLightViewProjectionMatrix(DirectX::FXMMATRIX transformMatrix) -> DirectX::XMMATRIX
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * g_lightProjectionMatrix;
}
} // namespace spotlight2

namespace nc::graphics
{

// struct LightPropertyVisitor
// {
    

//     std::vector<LightData>& data;
//     Transform* transform = nullptr;

//     void operator()(const DirectionalLightProperties& properties)
//     {
//         data.emplace_back(properties.color, transform->Forward());
//     }

//     void operator()(const PointLightProperties& properties)
//     {
//         data.emplace_back(
//             properties.color,
//             transform->Position(),
//             0,
//             properties.radius,
//             pointlight2::CalculateLightViewProjectionMatrix(transform->TransformationMatrix())
//         );
//     }

//     void operator()(const SpotLightProperties& properties)
//     {
//         data.emplace_back(
//             properties.color,
//             transform->Position(),
//             properties.innerAngle,
//             transform->Forward(),
//             properties.outerAngle,
//             properties.radius,
//             0,
//             spotlight2::CalculateLightViewProjectionMatrix(transform->TransformationMatrix())
//         );
//     }

// };

auto LightSubsystem::BuildState(ecs::ExplicitEcs<DirectionalLight, PointLight, SpotLight, Light, Transform> ecs) -> LightRenderState
{
    m_data.clear();

    // auto visitor = LightPropertyVisitor{m_data};
    // const auto& pool = ecs.GetPool<Light>();
    // for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
    // {
    //     visitor.transform = &ecs.Get<Transform>(entity);
    //     std::visit(visitor, light.properties);
    // }


    { // Directional Lights
        const auto& pool = ecs.GetPool<DirectionalLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(light.color, transform.Forward());
        }
    }

    { // Point Lights
        const auto& pool = ecs.GetPool<PointLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(
                light.diffuseColor,
                transform.Position(),
                0,
                light.radius,
                pointlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix())
            );
        }
    }

    { // Spot Lights
        const auto& pool = ecs.GetPool<SpotLight>();
        for (auto [entity, light] : std::views::zip(pool.GetEntityPool(), pool.GetComponents()))
        {
            auto& transform = ecs.Get<Transform>(entity);
            m_data.emplace_back(
                light.color,
                transform.Position(),
                light.innerAngle,
                transform.Forward(),
                light.outerAngle,
                light.radius,
                0,
                spotlight2::CalculateLightViewProjectionMatrix(transform.TransformationMatrix())
            );
        }
    }

    return LightRenderState{m_data};
}
} // namespace nc::graphics
