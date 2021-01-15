#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "imgui/imgui.h"
#endif

namespace
{
    const auto CubeMeshPath = std::string{"project/Models/cube.fbx"};
    const auto SphereMeshPath = std::string{"project/Models/sphere.fbx"};
    auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::WireframeTechnique>;

    auto CreateBoxModel()
    {
        return nc::graphics::Model{ {CubeMeshPath}, CreateMaterial() };
    }

    auto CreateSphereModel()
    {
        return nc::graphics::Model{ {SphereMeshPath}, CreateMaterial() };
    }
}

namespace nc
{
    ColliderBase::ColliderBase(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale, graphics::Model model)
        : Component(handle, parentHandle),
          m_data{Ecs::GetComponent<Transform>(parentHandle), scale, std::move(model)}
    {}

    ColliderBase::~ColliderBase() = default;

    void ColliderBase::Update(graphics::FrameManager& frame)
    {
        IF_THROW(!m_data.transform, "ColliderBase::Update - Bad Transform Ptr");
        m_data.model.SetTransformationMatrix(m_data.transform->GetTransformationMatrixEx(m_data.scale));
        m_data.model.Submit(frame);
    }

    DirectX::FXMMATRIX ColliderBase::GetTransformationMatrix() const
    {
        return m_data.model.GetTransformationMatrix();
    }


    #ifdef NC_EDITOR_ENABLED
    void ColliderBase::EditorGuiElement()
    {
        const float itemWidth = 80.0f;
        const float dragSpeed = 1.0f;

        ImGui::PushItemWidth(itemWidth);
            ImGui::Spacing(); ImGui::Separator(); ImGui::Text("Collider");
            ImGui::BeginGroup();
                ImGui::Indent();    ImGui::Text("Scale");
                ImGui::Text("X:");  ImGui::SameLine();  ImGui::SliderFloat("##colliderxscale", &m_data.scale.x, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Y:");  ImGui::SameLine();  ImGui::SliderFloat("##collideryscale", &m_data.scale.y, 0.01f, 100.0f, "%.1f", dragSpeed);  ImGui::SameLine();
                ImGui::Text("Z:");  ImGui::SameLine();  ImGui::SliderFloat("##colliderzscale", &m_data.scale.z, 0.01f, 100.0f, "%.1f", dragSpeed);
            ImGui::EndGroup();
            ImGui::Separator();
        ImGui::PopItemWidth();
    }
    #endif

    BoxCollider::BoxCollider(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale)
        : ColliderBase{handle, parentHandle, scale, CreateBoxModel()}
    {}

    BoxCollider::~BoxCollider() = default;

    SphereCollider::SphereCollider(ComponentHandle handle, EntityHandle parentHandle, Vector3 scale)
        : ColliderBase{handle, parentHandle, scale, CreateSphereModel()}
    {}

    SphereCollider::~SphereCollider() = default;
}