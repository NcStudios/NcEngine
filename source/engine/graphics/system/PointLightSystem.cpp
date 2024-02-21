#include "PointLightSystem.h"
#include "graphics/shader_resource/PointLightData.h"

#include "optick.h"

namespace
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
} // anonymous namespace

namespace nc::graphics
{
PointLightSystem::PointLightSystem(Signal<const std::vector<PointLightData>&>&& backendChannel, bool useShadows)
    : m_backendChannel{std::move(backendChannel)},
      m_useShadows{useShadows}
{
}

auto PointLightSystem::Execute(MultiView<PointLight, Transform> view) -> LightingState
{
    OPTICK_CATEGORY("PointLightSystem::Execute", Optick::Category::Rendering);
    auto state = LightingState{};
    state.viewProjections.clear();
    auto shaderBuffer = std::vector<PointLightData>{};
    shaderBuffer.reserve(view.size_upper_bound());

    for (const auto& [light, transform] : view)
    {
        state.viewProjections.push_back(::CalculateLightViewProjectionMatrix(transform->TransformationMatrix()));
        shaderBuffer.emplace_back(state.viewProjections.back(),
                                  transform->Position(),
                                  m_useShadows,
                                  light->ambientColor,
                                  light->diffuseColor,
                                  light->diffuseIntensity);
    }

    m_backendChannel.Emit(shaderBuffer);
    return state;
}
} // namespace nc::graphics
