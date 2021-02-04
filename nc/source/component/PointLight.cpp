#include "component/PointLight.h"
#include "graphics/Graphics.h"
#include "debug/Utils.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    PointLight::PointLight(EntityHandle handle) noexcept
        : ComponentBase(handle),
          m_transform{ GetComponent<Transform>(handle) }
    {
        PixelConstBufData.pos              = Vector3::Zero();
        PixelConstBufData.ambient          = Vector3::Splat(0.65f);
        PixelConstBufData.diffuseColor     = Vector3::Splat(1.0f);
        PixelConstBufData.diffuseIntensity = 0.9f;
        PixelConstBufData.attConst         = 2.61f;
        PixelConstBufData.attLin           = 0.1819f;
        PixelConstBufData.attQuad          = 0.0000001f;
    }

    void PointLight::Set(Vector3 pos, float radius, Vector3 ambient, Vector3 diffuseColor, float diffuseIntensity, float attConst, float attLin, float attQuad)
    {
        (void)radius; //currently unused
        PixelConstBufData.pos              = pos;
        PixelConstBufData.ambient          = ambient;
        PixelConstBufData.diffuseColor     = diffuseColor;
        PixelConstBufData.diffuseIntensity = diffuseIntensity;
        PixelConstBufData.attConst         = attConst;
        PixelConstBufData.attLin           = attLin;
        PixelConstBufData.attQuad          = attQuad;
    }

    #ifdef NC_EDITOR_ENABLED
    void PointLight::EditorGuiElement()
    {
        const float dragSpeed = 1.0f;

        ImGui::Text("Point Light");
        ImGui::Indent();
            ImGui::Text("Ambient    ");
            ImGui::Indent();
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##ambcolor", &PixelConstBufData.ambient.x, ImGuiColorEditFlags_NoInputs);
            ImGui::Unindent();
            ImGui::Text("Diffuse    ");
            ImGui::Indent();  
                ImGui::Text("Color      ");   ImGui::SameLine();  ImGui::ColorEdit3("##difcolor", &PixelConstBufData.diffuseColor.x, ImGuiColorEditFlags_NoInputs);
                ui::editor::floatWidget("Intensity ", "##diffuseintensity", &PixelConstBufData.diffuseIntensity, dragSpeed, 0.01f, 5.0f, "%.1f");
            ImGui::Unindent();
            ImGui::Text("Attenuation");
                ui::editor::columnHeaderWidget("", "Const", "Lin", "Quad");
                ui::editor::xyzWidget("", "att", &PixelConstBufData.attConst, &PixelConstBufData.attLin, &PixelConstBufData.attQuad, 0.01f, 1.0f);
        ImGui::Unindent();
    }
    #endif

    void PointLight::SetPositionFromCameraProjection(const DirectX::FXMMATRIX& view) noexcept(false)
    {
        IF_THROW(!m_transform, "PointLight::Bind - Bad Transform Ptr");
        
        PixelConstBufData.pos = m_transform->GetPosition();
        const auto pos_v = DirectX::XMLoadVector3(&PixelConstBufData.pos);
        DirectX::XMStoreVector3(&ProjectedPos, DirectX::XMVector3Transform(pos_v, view));
    }

}