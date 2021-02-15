#include "component/Collider.h"
#include "ColliderDetail.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    Collider::Collider(EntityHandle handle, ColliderType type, Vector3 scale)
        : ComponentBase(handle),
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_boundingVolume{collider_detail::CreateBoundingVolume(type, Vector3::Zero(), scale)}, /** @todo pass offset */
          m_type{type},
          m_widgetModel{collider_detail::CreateWireframeModel(type)},
          m_scale{scale},
          m_selectedInEditor{false}
    {
        IF_THROW(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f, "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(type == ColliderType::Sphere && scale - scale != Vector3::Zero(), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #else
    Collider::Collider(EntityHandle handle, ColliderType type, Vector3 scale)
        : ComponentBase(handle),
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_boundingVolume{collider_detail::CreateBoundingVolume(type, Vector3::Zero(), scale)}, /** @todo pass offset */
          m_type{type}
    {
        IF_THROW(scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f, "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(type == ColliderType::Sphere && scale - scale != Vector3::Zero(), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #endif

    Collider::~Collider() = default;

    ColliderType Collider::GetType() const
    {
        return m_type;
    }

    Collider::BoundingVolume Collider::GetBoundingVolume() const
    {
        return std::visit([this](auto&& volume) 
        {
            using Volume_t = std::remove_cvref<decltype(volume)>::type;
            Volume_t out;
            volume.Transform(out, m_transformMatrix);
            return BoundingVolume{out};
        }, *m_boundingVolume);
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget(graphics::FrameManager& frame)
    {
        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
            return;

        /** @todo right multiply by offset */
        m_widgetModel.SetTransformationMatrix
        (
            DirectX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z) *
            m_transformMatrix
        );

        m_widgetModel.Submit(frame);
    }

    void Collider::EditorGuiElement()
    {
        ImGui::Text("Collider");
        ui::editor::xyzWidgetHeader("     ");
        /** @todo offset */
        ui::editor::xyzWidget("Scale", "colliderscale", &m_scale.x, &m_scale.y, &m_scale.z, 0.01f, 100.0f);
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
    }
    #endif
}