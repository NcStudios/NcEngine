#include "component/PointLight.h"
#include "component/Transform.h"
#include "config/Config.h"
#include "Ecs.h"

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
    constexpr float LIGHT_FIELD_OF_VIEW = 45.0f;


}

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    void SerializeToFile(const std::string& filePath, const PointLightInfo& info) // @todo: remove or build out into formal asset generator
    {
        std::ofstream pointLightDataFile(filePath);

        if (pointLightDataFile.is_open())
        {
            pointLightDataFile << "Pos: " << info.pos << '\n';
            pointLightDataFile << "Ambient: " << info.ambient << '\n';
            pointLightDataFile << "Diffuse: " << info.diffuseColor << '\n';
            pointLightDataFile << "Diffuse Intensity: " << info.diffuseIntensity << '\n';
            pointLightDataFile << "Attenuation Constant: " << info.attConst << '\n';
            pointLightDataFile << "Attenuation Linear: " << info.attLin << '\n';
            pointLightDataFile << "Attenuation Quadratic: " << info.attQuad << '\n';
            pointLightDataFile.close();
        }
    }
    #endif

    PointLight::PointLight(Entity entity, PointLightInfo info)
        : ComponentBase{entity},
          m_info{info},
          m_lightProjectionMatrix{},
          m_isDirty{false}
    {
        const auto& graphicsSettings = config::GetGraphicsSettings();
        m_lightProjectionMatrix = DirectX::XMMatrixPerspectiveRH(math::DegreesToRadians(LIGHT_FIELD_OF_VIEW), 1.0f, graphicsSettings.nearClip, graphicsSettings.farClip); // global variable todo anonymous namespace const var. //make clips global constants in source
    }

    void PointLight::SetInfo(PointLightInfo info)
    {
        m_isDirty = true;
        m_info = info;
    }

    DirectX::XMMATRIX PointLight::CalculateLightViewProjectionMatrix()
    {
        DirectX::XMVECTOR scl_v, rot_v, pos_v;
        DirectX::XMMatrixDecompose(&scl_v, &rot_v, &pos_v, ActiveRegistry()->Get<Transform>(GetParentEntity())->GetTransformationMatrix());
        auto look_v = DirectX::XMVector3Transform(DirectX::g_XMIdentityR2, DirectX::XMMatrixRotationQuaternion(rot_v));
        return DirectX::XMMatrixLookAtRH(pos_v, pos_v + look_v, DirectX::g_XMNegIdentityR1) * m_lightProjectionMatrix;
    }

    bool PointLight::Update(const Vector3& position, const DirectX::XMMATRIX& cameraView, const DirectX::XMMATRIX& lightViewProj)
    {     
        Vector3 projectedPos;
        m_info.viewProjection = lightViewProj;

        const auto pos_v = DirectX::XMLoadVector3(&position);
        DirectX::XMStoreVector3(&projectedPos, DirectX::XMVector3Transform(pos_v, cameraView));
        
        if (projectedPos.x != m_info.pos.x || projectedPos.y != m_info.pos.y || projectedPos.z != m_info.pos.z)
        {
            m_isDirty = true;
        }

        m_info.pos = projectedPos;

        return std::exchange(m_isDirty, false);
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