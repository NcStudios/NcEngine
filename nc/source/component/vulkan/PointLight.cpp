#pragma once

#include "component/vulkan/PointLight.h"
#include "directx/math/DirectXMath.h"

namespace nc::vulkan
{
    #ifdef NC_EDITOR_ENABLED
    void PointLight::EditorGuiElement()
    {
        const float dragSpeed = 1.0f;

        ImGui::Text("Point Light");
        ImGui::Indent();
            ImGui::Text("Ambient    ");
            ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor", &m_info.ambient.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Diffuse    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor", &m_info.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ui::editor::floatWidget("Intensity ", "##diffuseintensity", &m_info.diffuseIntensity, dragSpeed, 0.01f, 5.0f, "%.1f");
            ImGui::Unindent();
            ImGui::Text("Attenuation");
                ui::editor::columnHeaderWidget("", "Const", "Lin", "Quad");
                ui::editor::xyzWidget("", "att", &m_info.attConst, &m_info.attLin, &m_info.attQuad, 0.01f, 1.0f);
        ImGui::Unindent();
    }
    #endif

    PointLight::PointLight(EntityHandle parentHandle)
    : Component{parentHandle}
    {
    }

    PointLight::PointLight(EntityHandle parentHandle, const PointLightInfo& info)
    : Component{parentHandle},
      m_info{info}
    {
    }

    const PointLightInfo& PointLight::GetInfo()
    {
        return m_info;
    }

    void PointLight::Update(const PointLightInfo& info)
    {
        m_info.ambient = info.ambient;
        m_info.attConst = info.attConst;
        m_info.attLin = info.attLin;
        m_info.attQuad = info.attQuad;
        m_info.diffuseColor = info.diffuseColor;
        m_info.diffuseIntensity = info.diffuseIntensity;
    }
}