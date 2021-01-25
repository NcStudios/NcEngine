#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
    const auto ColliderMesh = std::string{"project/Models/cube.fbx"};
    auto GetColliderMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::WireframeTechnique>;
}

namespace nc
{
    Collider::Collider(EntityHandle handle, Vector3 scale)
        : ComponentBase(handle),
          m_transform{Ecs::GetComponent<Transform>(handle)},
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
        ImGui::Text("Collider");
        ui::editor::xyzWidgetHeader("     ");
        ui::editor::xyzWidget("Scale", "colliderscale", &m_scale.x, &m_scale.y, &m_scale.z, 0.01f, 100.0f);
    }
    #endif
}