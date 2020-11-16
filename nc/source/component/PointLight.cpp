#include "PointLight.h"
#include "graphics/Graphics.h"
#include "NcDebug.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{

    PointLight::PointLight()
        : m_transform{ nullptr }
    {
        m_pixelConstBufData.pos              = {0,0,0};
        m_pixelConstBufData.ambient          = {0.65f, 0.65f, 0.65f};
        m_pixelConstBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        m_pixelConstBufData.diffuseIntensity = 0.9f;
        m_pixelConstBufData.attConst         = 2.61f;
        m_pixelConstBufData.attLin           = 0.1819f;
        m_pixelConstBufData.attQuad          = 0.0000001f;
    }

    PointLight::~PointLight() = default;
    
    PointLight::PointLight(PointLight&& other)
        : m_pixelConstBufData { other.m_pixelConstBufData }
    {
    }

    PointLight & PointLight::operator=(PointLight&& other)
    {
        m_pixelConstBufData = other.m_pixelConstBufData;
        return *this;
    }

    void PointLight::Set(DirectX::XMFLOAT3 pos, float radius)
    {
        (void)radius; //currently unused
        m_pixelConstBufData.pos              = pos;
        m_pixelConstBufData.ambient          = {0.65f, 0.65f, 0.65f};
        m_pixelConstBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        m_pixelConstBufData.diffuseIntensity = 0.9f;
        m_pixelConstBufData.attConst         = 2.61f;
        m_pixelConstBufData.attLin           = 0.1819f;
        m_pixelConstBufData.attQuad          = 0.0000001f;
    }

    #ifdef NC_EDITOR_ENABLED
    void PointLight::EditorGuiElement()
    {
        const float itemWidth = 80.0f;
        const float dragSpeed = 1.0f;

        ImGui::PushItemWidth(itemWidth);    ImGui::Spacing();   ImGui::Separator();
            ImGui::Text("Point Light");
            ImGui::Indent();
                ImGui::Text("Ambient    ");
                ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor", &m_pixelConstBufData.ambient.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Unindent();
                ImGui::Text("Diffuse    ");
                ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor", &m_pixelConstBufData.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Text("Intensity  ");   ImGui::SameLine();  ImGui::SliderFloat( "##diffuseintensity", &m_pixelConstBufData.diffuseIntensity, 0.01f, 5.0f, "%.2f", dragSpeed);
                ImGui::Unindent();
                ImGui::Text("Attenuation");
                ImGui::Indent();
                ImGui::Text("Constant   ");   ImGui::SameLine();  ImGui::SliderFloat("##attconst", &m_pixelConstBufData.attConst, 0.001f, 1.0f, "%.2f", dragSpeed);
                ImGui::Text("Linear     ");   ImGui::SameLine();  ImGui::SliderFloat("##attlin", &m_pixelConstBufData.attLin, 0.001f,  1.0f, "%.2f", dragSpeed);
                ImGui::Text("Quadratic  ");   ImGui::SameLine();  ImGui::SliderFloat("##attquad", &m_pixelConstBufData.attQuad, 0.001f, 1.0f, "%.2f", dragSpeed);
                ImGui::Unindent();
            ImGui::Unindent();
        ImGui::Separator();  ImGui::PopItemWidth();
    }
    #endif

    void PointLight::SetPositionFromCameraProjection(DirectX::FXMMATRIX view) noexcept(false)
    {
        if (!m_transform)
        {
            m_transform = Ecs::GetComponent<Transform>(m_parentHandle);
        }
        IF_THROW(!m_transform, "PointLight::Bind - Bad Transform Ptr");
        
        m_pixelConstBufData.pos = m_transform->GetPosition().GetXMFloat3();
        const auto pos = DirectX::XMLoadFloat3(&m_pixelConstBufData.pos);
        DirectX::XMStoreFloat3(&projectedPos, DirectX::XMVector3Transform(pos, view));
    }

}