#include "component/vulkan/PointLight.h"
#include "component/Transform.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc::vulkan
{
    Attenuation GetAttenuationFromRange(float range)
    {
        Attenuation att = {};
        att.constant = 1.0f;
        att.linear = 4.5f / range;
        att.quadratic = 75.0f / (range * range);
        return att;
    }

    #ifdef USE_VULKAN
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
            ImGui::Unindent();
            ImGui::Text("Specular    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##specularcolor", &m_info.specularColor.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Intensity");
            ImGui::Indent();  
                ui::editor::floatWidget("Intensity", "difintensity", &m_info.diffuseIntensity, dragSpeed,  0.0f, 600.0f, "%.2f");
                ui::editor::floatWidget("Range", "range", &m_range, dragSpeed,  0.0f, 600.0f, "%.2f");
            ImGui::Unindent();
        ImGui::Unindent();
    }
    #endif
    #endif

    PointLight::PointLight(EntityHandle parentHandle, PointLightInfo info)
    : Component{parentHandle},
      m_range{13},
      m_info{info}
    {
        auto attenuation = GetAttenuationFromRange(m_range);
        m_info.attConst = attenuation.constant;
        m_info.attLin = attenuation.linear;
        m_info.attQuad = attenuation.quadratic;
    }

    const PointLightInfo& PointLight::GetInfo()
    {
        return m_info;
    }

    void PointLight::Update()
    {
        auto transformPos = GetComponent<Transform>(GetParentHandle())->GetPosition();
        auto attenuation = GetAttenuationFromRange(m_range);
        m_info.attConst = attenuation.constant;
        m_info.attLin = attenuation.linear;
        m_info.attQuad = attenuation.quadratic;
        m_info.pos = Vector4(transformPos.x, transformPos.y, transformPos.z, 1.0);
        m_info.isInitialized = true;
    }

    void PointLight::Set(const PointLightInfo& info)
    {
        m_info.pos = info.pos;
        m_info.ambient = info.ambient;
        m_info.attConst = info.attConst;
        m_info.attLin = info.attLin;
        m_info.attQuad = info.attQuad;
        m_info.diffuseColor = info.diffuseColor;
        m_info.specularColor = info.specularColor;
        m_info.isInitialized = true;
    }
}