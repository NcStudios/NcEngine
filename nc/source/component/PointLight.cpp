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
        : m_transform{ nullptr }
    {
        PixelConstBufData.pos              = {0,0,0};
        PixelConstBufData.ambient          = {0.65f, 0.65f, 0.65f};
        PixelConstBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        PixelConstBufData.diffuseIntensity = 0.9f;
        PixelConstBufData.attConst         = 2.61f;
        PixelConstBufData.attLin           = 0.1819f;
        PixelConstBufData.attQuad          = 0.0000001f;
    }

    PointLight::~PointLight() = default;
    
    PointLight::PointLight(PointLight&& other)
        : PixelConstBufData { other.PixelConstBufData }
    {
    }

    PointLight & PointLight::operator=(PointLight&& other)
    {
        PixelConstBufData = other.PixelConstBufData;
        return *this;
    }

    void PointLight::Set(DirectX::XMFLOAT3 pos, float radius)
    {
        (void)radius; //currently unused
        PixelConstBufData.pos              = pos;
        PixelConstBufData.ambient          = {0.65f, 0.65f, 0.65f};
        PixelConstBufData.diffuseColor     = {1.0f, 1.0f, 1.0f};
        PixelConstBufData.diffuseIntensity = 0.9f;
        PixelConstBufData.attConst         = 2.61f;
        PixelConstBufData.attLin           = 0.1819f;
        PixelConstBufData.attQuad          = 0.0000001f;
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
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor", &PixelConstBufData.ambient.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Unindent();
                ImGui::Text("Diffuse    ");
                ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor", &PixelConstBufData.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Text("Intensity  ");   ImGui::SameLine();  ImGui::SliderFloat( "##diffuseintensity", &PixelConstBufData.diffuseIntensity, 0.01f, 5.0f, "%.2f", dragSpeed);
                ImGui::Unindent();
                ImGui::Text("Attenuation");
                ImGui::Indent();
                ImGui::Text("Constant   ");   ImGui::SameLine();  ImGui::SliderFloat("##attconst", &PixelConstBufData.attConst, 0.001f, 1.0f, "%.2f", dragSpeed);
                ImGui::Text("Linear     ");   ImGui::SameLine();  ImGui::SliderFloat("##attlin", &PixelConstBufData.attLin, 0.001f,  1.0f, "%.2f", dragSpeed);
                ImGui::Text("Quadratic  ");   ImGui::SameLine();  ImGui::SliderFloat("##attquad", &PixelConstBufData.attQuad, 0.001f, 1.0f, "%.2f", dragSpeed);
                ImGui::Unindent();
            ImGui::Unindent();
        ImGui::Separator();  ImGui::PopItemWidth();
    }
    #endif

    void PointLight::SetPositionFromCameraProjection(const DirectX::FXMMATRIX& view) noexcept(false)
    {
        if (!m_transform)
        {
            m_transform = Ecs::GetComponent<Transform>(m_parentHandle);
        }
        IF_THROW(!m_transform, "PointLight::Bind - Bad Transform Ptr");
        
        PixelConstBufData.pos = m_transform->GetPosition().ToXMFloat3();
        const auto pos = DirectX::XMLoadFloat3(&PixelConstBufData.pos);
        DirectX::XMStoreFloat3(&ProjectedPos, DirectX::XMVector3Transform(pos, view));
    }

}