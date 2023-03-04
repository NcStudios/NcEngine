#include "config/Config.h"
#include "ecs/Transform.h"
#include "ecs/Registry.h"
#include "graphics/PointLight.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#include <iostream>
#include <fstream>
#include <debug/Serialize.h>
#endif

#include <cassert>

namespace
{
// Angle of light's field of view.
constexpr float LightFieldOfView = nc::DegreesToRadians(60.0f);

// Near clip of light's frustum
constexpr float NearClip = 0.25f;

// Far clip of light's frustum
constexpr float FarClip = 96.0f;
} // anonymous namespace

namespace nc::graphics
{
PointLight::PointLight(Entity entity, PointLightInfo info)
    : ComponentBase{entity},
        m_info{info},
        m_lightProjectionMatrix{DirectX::XMMatrixPerspectiveRH(LightFieldOfView, 1.0f, NearClip, FarClip)},
        m_isDirty{false}
{
    m_info.castShadows = config::GetGraphicsSettings().useShadows;
}

void PointLight::SetInfo(const PointLightInfo& info)
{
    m_isDirty = true;
    m_info = info;
}

/** The editor modifies m_info directly so we need a way to update just the flag. */
void PointLight::SetDirty()
{
    m_isDirty = true;
}

DirectX::XMMATRIX PointLight::CalculateLightViewProjectionMatrix(const DirectX::XMMATRIX& transformMatrix)
{
    const auto look = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, transformMatrix);
    return DirectX::XMMatrixLookAtRH(transformMatrix.r[3], look, DirectX::g_XMNegIdentityR1) * m_lightProjectionMatrix;
}

bool PointLight::Update(const Vector3& position, const DirectX::XMMATRIX& lightViewProj)
{
    m_info.viewProjection = lightViewProj;

    if (position.x != m_info.pos.x || position.y != m_info.pos.y || position.z != m_info.pos.z)
    {
        m_isDirty = true;
    }

    m_info.pos = position;
    return std::exchange(m_isDirty, false);
}
} // namespace nc::graphics

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::PointLight>(graphics::PointLight* light)
{
    const float dragSpeed = 1.0f;
    auto& info = light->GetInfo();

    Vector3 ambient = info.ambient;
    Vector3 diffuse = info.diffuseColor;
    float diffuseIntensity = info.diffuseIntensity;

    ImGui::Text("Point Light");
    ImGui::Indent();
        ImGui::Text("Ambient    ");
        ImGui::Indent();
            ImGui::Text("Color      ");   ImGui::SameLine();  auto ambientResult = ImGui::ColorEdit3("##ambcolor", &ambient.x, ImGuiColorEditFlags_NoInputs);
        ImGui::Unindent();
        ImGui::Text("Diffuse    ");
        ImGui::Indent();  
            ImGui::Text("Color      ");   ImGui::SameLine(); auto diffuseResult = ImGui::ColorEdit3("##difcolor", &diffuse.x, ImGuiColorEditFlags_NoInputs);
            auto diffuseIntensityResult = ui::editor::floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 1200.0f, "%.2f");
        ImGui::Unindent();
    ImGui::Unindent();

    auto pointLightInfo = info;

    if (ambientResult) pointLightInfo.ambient = ambient;
    if (diffuseResult) pointLightInfo.diffuseColor = diffuse;
    if (diffuseIntensityResult) pointLightInfo.diffuseIntensity = diffuseIntensity;

    if (ambientResult || diffuseResult || diffuseIntensityResult)
    {
        light->SetInfo(pointLightInfo);
    }
}
#endif
} // namespace nc
