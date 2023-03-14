#include "graphics/PointLight.h"

#ifdef NC_EDITOR_ENABLED
#include "debug/Serialize.h"
#include "ui/editor/Widgets.h"
#include <iostream>
#include <fstream>
#endif

namespace nc
{
#ifdef NC_EDITOR_ENABLED
template<> void ComponentGuiElement<graphics::PointLight>(graphics::PointLight* light)
{
    const float dragSpeed = 1.0f;

    Vector3 ambient = light->GetAmbient();
    Vector3 diffuse = light->GetDiffuseColor();
    float diffuseIntensity = light->GetDiffuseIntensity();

    ImGui::Text("Point Light");
    ImGui::Indent();
        ImGui::Text("Ambient    ");
        ImGui::Indent();
            ImGui::Text("Color      ");   ImGui::SameLine();  auto ambientResult = ImGui::ColorEdit3("##ambcolor", &ambient.x, ImGuiColorEditFlags_NoInputs);
        ImGui::Unindent();
        ImGui::Text("Diffuse    ");
        ImGui::Indent();  
            ImGui::Text("Color      ");   ImGui::SameLine(); auto diffuseResult = ImGui::ColorEdit3("##difcolor", &diffuse.x, ImGuiColorEditFlags_NoInputs);
            auto diffuseIntensityResult = ui::editor::floatWidget("Intensity", "difintensity", &diffuseIntensity, dragSpeed,  0.0f, 1200.0f, "%.2f");
        ImGui::Unindent();
    ImGui::Unindent();

    if (ambientResult) light->SetAmbient(ambient);
    if (diffuseResult) light->SetDiffuseColor(diffuse);
    if (diffuseIntensityResult) light->SetDiffuseIntensity(diffuseIntensity);
}
#endif
} // namespace nc
