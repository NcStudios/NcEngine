#include "PointLight.h"
#include "graphics/Graphics.h"
#include "utils/editor/EditorManager.h"
#include "NCE.h"

#ifdef NC_EDITOR_ENABLED
#include "external/imgui/imgui.h"
#endif

namespace nc
{
    PointLight::PointLight()
    {}

    void PointLight::Set(DirectX::XMFLOAT3 pos, float radius)
    {
        (void)radius; //currently unused
        m_cBuf                          = std::make_unique<PixelConstBuf>();
        m_constBufData.pos              = pos;
        m_constBufData.ambient          = {0.05f, 0.05f, 0.05f};
        m_constBufData.diffuseColor     = {0.8f, 0.2f, 0.2f};
        m_constBufData.diffuseIntensity = 1.2f;
        m_constBufData.attConst         = 0.09f;
        m_constBufData.attLin           = 0.012f;
        m_constBufData.attQuad          = 0.0075f;
    }

    #ifdef NC_EDITOR_ENABLED
    void PointLight::EditorGuiElement()
    {
        const float itemWidth = 40.0f;
        const float dragSpeed = 0.75f;

        ImGui::PushItemWidth(itemWidth);    ImGui::Spacing();   ImGui::Separator();
            ImGui::Text("Point Light");
            ImGui::Indent();
                ImGui::Text("Ambient    ");
                ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor",         &m_constBufData.ambient.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Unindent();
                ImGui::Text("Diffuse    ");
                ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor",         &m_constBufData.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ImGui::Text("Intensity  ");   ImGui::SameLine();  ImGui::DragFloat( "##diffuseintensity", &m_constBufData.diffuseIntensity, dragSpeed, 0.01f, 2.0f, "%.05f");
                ImGui::Unindent();
                ImGui::Text("Attenuation");
                ImGui::Indent();
                ImGui::Text("Constant   ");   ImGui::SameLine();  ImGui::DragFloat("##attconst",          &m_constBufData.attConst, dragSpeed,         0.05f, 10.0f, "%.2f", 2);
                ImGui::Text("Linear     ");   ImGui::SameLine();  ImGui::DragFloat("##attlin",            &m_constBufData.attLin,   dragSpeed,       0.0001f,  4.0f, "%.4f", 4);
                ImGui::Text("Quadratic  ");   ImGui::SameLine();  ImGui::DragFloat("##attquad",           &m_constBufData.attQuad,  dragSpeed,    0.0000001f, 10.0f, "%.7f", 8);
                ImGui::Unindent();
            ImGui::Unindent();
        ImGui::Separator();  ImGui::PopItemWidth();
    }
    #endif

    void PointLight::Bind(DirectX::FXMMATRIX view) noexcept(false)
    {
        auto trans = NCE::GetTransform(*GetParentView());
        IF_THROW(trans == nullptr, "PointLight::Bind - Bad Transform Ptr");
        m_constBufData.pos = trans->GetPosition().GetXMFloat3();
        PointLightCBuf cBufCopy = m_constBufData;
        const auto pos = DirectX::XMLoadFloat3(&m_constBufData.pos);
        DirectX::XMStoreFloat3(&cBufCopy.pos, DirectX::XMVector3Transform(pos, view));
        m_cBuf->Update(cBufCopy);
        m_cBuf->Bind();
    }

}