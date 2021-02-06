#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"

namespace
{
    const auto CubeMeshPath = std::string{"project/Models/cube.fbx"};
    auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>;

    auto CreateWireframeBox()
    {
        return nc::graphics::Model{ {CubeMeshPath}, CreateMaterial() };
    }
}
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    Collider::Collider(EntityHandle handle, Vector3 scale)
        : ComponentBase(handle),
          m_colliderMatrix{},
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_scale{scale},
          m_widgetModel{CreateWireframeBox()},
          m_selectedInEditor{false}
    {
    }
    #else
    Collider::Collider(EntityHandle handle, Vector3 scale)
        : ComponentBase(handle),
          m_colliderMatrix{},
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_scale{scale}
    {
    }
    #endif

    Collider::~Collider() = default;

    void Collider::UpdateTransformationMatrix()
    {
        IF_THROW(m_scale == Vector3::Zero(), "Collider::UpdateTransformationMatrix - Scale cannot be zero");
        m_colliderMatrix = DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) * m_transformMatrix;
    }

    DirectX::FXMMATRIX Collider::GetTransformationMatrix() const
    {
        return m_colliderMatrix;
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget(graphics::FrameManager& frame)
    {
        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
            return;

        m_widgetModel.SetTransformationMatrix(m_colliderMatrix);
        m_widgetModel.Submit(frame);
    }

    void Collider::EditorGuiElement()
    {
        ImGui::Text("Collider");
        ui::editor::xyzWidgetHeader("     ");
        ui::editor::xyzWidget("Scale", "colliderscale", &m_scale.x, &m_scale.y, &m_scale.z, 0.01f, 100.0f);
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
    }
    #endif
}