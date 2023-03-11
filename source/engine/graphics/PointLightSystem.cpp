#include "PointLightSystem.h"
#include "shader_resource/PointLightData.h"
#include "shader_resource/ShaderResourceService.h"

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
PointLightSystem::PointLightSystem(bool useShadows)
    : m_viewProjections{},
      m_useShadows{useShadows}
{
}

void PointLightSystem::Update(MultiView<PointLight, Transform> view)
{
    m_viewProjections.clear();
    auto shaderBuffer = std::vector<PointLightData>{};
    shaderBuffer.reserve(view.size_upper_bound());

    for (const auto& [light, transform] : view)
    {
        m_viewProjections.push_back(::CalculateLightViewProjectionMatrix(transform->TransformationMatrix()));
        shaderBuffer.emplace_back(m_viewProjections.back(),
                                  transform->Position(),
                                  m_useShadows,
                                  light->GetAmbient(),
                                  light->GetDiffuseColor(),
                                  light->GetDiffuseIntensity());
    }

    ShaderResourceService<PointLightData>::Get()->Update(shaderBuffer);
}

void PointLightSystem::Clear() noexcept
{
    m_viewProjections.clear();
    m_viewProjections.shrink_to_fit();
}
} // namespace nc::graphics
