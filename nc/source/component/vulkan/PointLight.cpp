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

    PointLight::PointLight(Entity entity, PointLightInfo info)
    : ComponentBase{entity},
      m_range{13},
      m_info{info}
    {
        auto attenuation = GetAttenuationFromRange(m_range);
        m_info.attConst = attenuation.constant;
        m_info.attLin = attenuation.linear;
        m_info.attQuad = attenuation.quadratic;
    }

    const PointLightInfo& PointLight::GetInfo() const
    {
        return m_info;
    }

    float PointLight::GetRange() const
    {
        return m_range;
    }

    void PointLight::Update()
    {
        auto transformPos = ActiveRegistry()->Get<Transform>(GetParentEntity())->GetPosition();
        auto attenuation = GetAttenuationFromRange(m_range);
        m_info.attConst = attenuation.constant;
        m_info.attLin = attenuation.linear;
        m_info.attQuad = attenuation.quadratic;
        m_info.pos = Vector4(transformPos.x, transformPos.y, transformPos.z, 1.0);
        m_info.isInitialized = true;
    }

    void PointLight::SetInfo(PointLightInfo info)
    {
        m_info = info;
        m_info.isInitialized = true;
    }

    void PointLight::SetRange(float range)
    {
        m_range = range;
    }
}

namespace nc
{
    #ifdef USE_VULKAN
    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<vulkan::PointLight>(vulkan::PointLight* light)
    {
        auto& info = light->GetInfo();
        auto range = light->GetRange();

        Vector4 ambient = info.ambient;
        Vector4 diffuse = info.diffuseColor;
        Vector4 specular = info.specularColor;
        float diffuseIntensity = info.diffuseIntensity;

        const float dragSpeed = 1.0f;
        ImGui::Text("Point Light");
        ImGui::Indent();
            ImGui::Text("Ambient    ");
            ImGui::Indent();
                ImGui::Text("Color      ");   
                ImGui::SameLine();  
                auto ambientResult = ImGui::ColorEdit3("##ambcolor", &ambient.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Diffuse    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   
                ImGui::SameLine();  
                auto diffuseResult = ImGui::ColorEdit3("##difcolor", &diffuse.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Specular    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");
                ImGui::SameLine();
                auto specularResult = ImGui::ColorEdit3("##specularcolor", &specular.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Intensity");
            ImGui::Indent();  
                auto diffuseIntensityResult = ui::editor::floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 600.0f, "%.2f");
                auto rangeResult = ui::editor::floatWidget("Range", "range", &range, dragSpeed,  0.0f, 600.0f, "%.2f");
            ImGui::Unindent();
        ImGui::Unindent();

        if (rangeResult) light->SetRange(range);

        auto pointLightInfo = info;

        if (ambientResult) pointLightInfo.ambient = ambient;
        if (diffuseResult) pointLightInfo.diffuseColor = diffuse;
        if (specularResult) pointLightInfo.specularColor = specular;
        if (diffuseIntensityResult) pointLightInfo.diffuseIntensity = diffuseIntensity;

        light->SetInfo(pointLightInfo);

    }
    #endif
    #endif
}