#include "component/PointLight.h"
#include "component/Transform.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#include <iostream>
#include <fstream>
#include <debug/Serialize.h>
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    void SerializeToFile(const std::string& filePath, const PointLightInfo& info) // @todo: remove or build out into formal asset generator
    {
        std::ofstream pointLightDataFile(filePath);

        if (pointLightDataFile.is_open())
        {
            pointLightDataFile << "Pos: " << info.pos << std::endl;
            pointLightDataFile << "Ambient: " << info.ambient << std::endl;
            pointLightDataFile << "Diffuse: " << info.diffuseColor << std::endl;
            pointLightDataFile << "Diffuse Intensity: " << info.diffuseIntensity << std::endl;
            pointLightDataFile << "Attenuation Constant: " << info.attConst << std::endl;
            pointLightDataFile << "Attenuation Linear: " << info.attLin << std::endl;
            pointLightDataFile << "Attenuation Quadratic: " << info.attQuad << std::endl;
            pointLightDataFile.close();
        }
    }
    #endif

    PointLight::PointLight(Entity entity, PointLightInfo info)
    : ComponentBase{entity},
      m_info{info},
      m_projectedPos{},
      m_isDirty{false}
    {
    }

    const PointLightInfo& PointLight::GetInfo() const
    {
        return m_info;
    }

    bool PointLight::IsDirty() const
    {
        return m_isDirty;
    }

    bool PointLight::Update(const DirectX::XMMATRIX& view)
    {        
        auto transformPos = ActiveRegistry()->Get<Transform>(GetParentEntity())->GetPosition();

        if (transformPos.x != m_info.pos.x || transformPos.y != m_info.pos.y || transformPos.z != m_info.pos.z)
        {
            m_isDirty = true;
        }

        const auto pos_v = DirectX::XMLoadVector3(&transformPos);
        DirectX::XMStoreVector3(&m_projectedPos, DirectX::XMVector3Transform(pos_v, view));

        m_info.pos.x = m_projectedPos.x;
        m_info.pos.y = m_projectedPos.y;
        m_info.pos.z = m_projectedPos.z;

        return std::exchange(m_isDirty, false);
    }

    void PointLight::SetInfo(PointLightInfo info)
    {
        m_isDirty = true;
        m_info = info;
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PointLight>(PointLight* light)
    {
        const float dragSpeed = 1.0f;
        auto& info = light->GetInfo();

        Vector3 ambient = info.ambient;
        Vector3 diffuse = info.diffuseColor;
        float diffuseIntensity = info.diffuseIntensity;
        float attConst = info.attConst;
        float attLin = info.attLin;
        float attQuad = info.attQuad;

        ImGui::Text("Point Light");
        ImGui::Indent();
            ImGui::Text("Ambient    ");
            ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  auto ambientResult = ImGui::ColorEdit3("##ambcolor", &ambient.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Diffuse    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine(); auto diffuseResult = ImGui::ColorEdit3("##difcolor", &diffuse.x, ImGuiColorEditFlags_NoInputs);
                auto diffuseIntensityResult = ui::editor::floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 600.0f, "%.2f");
            ImGui::Unindent();
            ImGui::Text("Attenuation");
                ui::editor::columnHeaderWidget("", "Const", "Lin", "Quad");
                ui::editor::xyzWidget("", "att", &attConst, &attLin, &attQuad, 0.01f, 1.0f);
                
            auto buttonSize = ImVec2{ImGui::GetWindowWidth() - 20, 18};
            if(ImGui::Button("Serialize", buttonSize))
                SerializeToFile("nc/PointLightData.txt", info);
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
}