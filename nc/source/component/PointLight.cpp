#include "component/PointLight.h"
#include "graphics/Graphics.h"
#include "debug/Utils.h"
#include "Ecs.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    PointLight::PointLight(Entity entity, Properties properties) noexcept
        : ComponentBase(entity),
          PixelConstBufData{properties},
          ProjectedPos{}
    {
    }

    void PointLight::Set(const PointLight::Properties& lightProperties)
    {
        PixelConstBufData = lightProperties;
    }

    void PointLight::SetPositionFromCameraProjection(const DirectX::FXMMATRIX& view)
    {
        auto* transform = ActiveRegistry()->Get<Transform>(GetParentEntity());
        PixelConstBufData.pos = transform->GetPosition();
        const auto pos_v = DirectX::XMLoadVector3(&PixelConstBufData.pos);
        DirectX::XMStoreVector3(&ProjectedPos, DirectX::XMVector3Transform(pos_v, view));
    }

    #ifdef NC_EDITOR_ENABLED
    template<> void ComponentGuiElement<PointLight>(PointLight* light)
    {
        const float dragSpeed = 1.0f;
        auto& PixelConstBufData = light->PixelConstBufData;

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
}