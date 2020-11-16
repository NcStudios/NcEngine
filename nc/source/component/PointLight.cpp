#include "PointLight.h"
#include "graphics/Graphics.h"
#include "DebugUtils.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace nc
{
    PointLight::PointLight()
        : m_cBuf{},
          m_transform{ nullptr }
    {
        m_constBufData.pos              = {0,0,0};
        m_constBufData.ambient          = {0.80f, 0.80f, 0.80f};
        m_constBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        m_constBufData.diffuseIntensity = 0.21515f;
        m_constBufData.attConst         = 1.41f;
        m_constBufData.attLin           = 0.0001f;
        m_constBufData.attQuad          = 0.0000001f;
    }

    PointLight::~PointLight() = default;
    
    PointLight::PointLight(PointLight&& other)
        : m_constBufData { other.m_constBufData },
          m_cBuf { std::move(other.m_cBuf) }
    {
    }

    PointLight & PointLight::operator=(PointLight&& other)
    {
        m_constBufData = other.m_constBufData;
        m_cBuf = std::move(other.m_cBuf);
        return *this;
    }

    void PointLight::Set(DirectX::XMFLOAT3 pos, float radius)
    {
        (void)radius; //currently unused
        m_constBufData.pos              = pos;
        m_constBufData.ambient          = {0.80f, 0.80f, 0.80f};
        m_constBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        m_constBufData.diffuseIntensity = 0.21515f;
        m_constBufData.attConst         = 1.41f;
        m_constBufData.attLin           = 0.0001f;
        m_constBufData.attQuad          = 0.0000001f;
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
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor", &m_constBufData.ambient.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Unindent();
                ImGui::Text("Diffuse    ");
                ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor", &m_constBufData.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Text("Intensity  ");   ImGui::SameLine();  ImGui::SliderFloat( "##diffuseintensity", &m_constBufData.diffuseIntensity, 0.1f, 2.0f, "%.05f", dragSpeed);
                ImGui::Unindent();
                ImGui::Text("Attenuation");
                ImGui::Indent();
                ImGui::Text("Constant   ");   ImGui::SameLine();  ImGui::SliderFloat("##attconst", &m_constBufData.attConst, 0.05f, 10.0f, "%.2f", dragSpeed);
                ImGui::Text("Linear     ");   ImGui::SameLine();  ImGui::SliderFloat("##attlin", &m_constBufData.attLin, 0.0001f,  4.0f, "%.4f", dragSpeed);
                ImGui::Text("Quadratic  ");   ImGui::SameLine();  ImGui::SliderFloat("##attquad", &m_constBufData.attQuad, 0.0000001f, 10.0f, "%.7f", dragSpeed);
                ImGui::Unindent();
            ImGui::Unindent();
        ImGui::Separator();  ImGui::PopItemWidth();
    }
    #endif

    void PointLight::Bind(DirectX::FXMMATRIX view) noexcept(false)
    {
        if (!m_transform)
        {
            m_transform = Ecs::GetComponent<Transform>(m_parentHandle);
        }
        IF_THROW(!m_transform, "PointLight::Bind - Bad Transform Ptr");
        
        m_constBufData.pos = m_transform->GetPosition().GetXMFloat3();
        PointLightCBuf cBufCopy = m_constBufData;
        const auto pos = DirectX::XMLoadFloat3(&m_constBufData.pos);
        DirectX::XMStoreFloat3(&cBufCopy.pos, DirectX::XMVector3Transform(pos, view));
        m_cBuf.Update(cBufCopy);
        m_cBuf.Bind();
    }

}