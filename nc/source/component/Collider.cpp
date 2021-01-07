#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace
{
    const auto ColliderMesh = std::string{"project/Models/cube.fbx"};
    auto GetColliderMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::WireframeTechnique>;
}

namespace nc
{
    Collider::Collider(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale)
        : Component(handle, parentHandle),
          m_transform{Ecs::GetComponent<Transform>(parentHandle)},
          m_scale{scale},
          m_model{ColliderMesh, GetColliderMaterial()}
    {}

    Collider::~Collider() = default;

    void Collider::Update(graphics::FrameManager& frame)
    {
        IF_THROW(!m_transform, "Collider::Update - Bad Transform Ptr");
        m_model.UpdateTransformationMatrix(m_transform->GetTransformationMatrixEx(m_scale));
        m_model.Submit(frame);
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::EditorGuiElement()
    {
        const float itemWidth = 80.0f;
        const float dragSpeed = 1.0f;

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing(); ImGui::Separator(); ImGui::Text("Collider");
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Scale");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##colliderxscale", &m_scale.x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##collideryscale", &m_scale.y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##colliderzscale", &m_scale.z, 0.01f, 100.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif
}