#include "component/Collider.h"
#include "ColliderDetail.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

#ifdef NC_EDITOR_ENABLED
namespace
{
    bool IsUniformScale(const nc::Vector3& scale)
    {
        return nc::math::FloatEqual(scale.x, scale.y) && nc::math::FloatEqual(scale.y, scale.z);
    }
}
#endif

namespace nc
{
    #ifdef NC_EDITOR_ENABLED
    Collider::Collider(EntityHandle handle, ColliderInfo info)
        : ComponentBase(handle),
          m_type{info.type},
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_boundingVolume{collider_detail::CreateBoundingVolume(info.type, info.offset, info.scale)},
          m_widgetModel{collider_detail::CreateWireframeModel(info.type)},
          m_selectedInEditor{false}
    {
        IF_THROW(HasAnyZeroElement(info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(info.type == ColliderType::Sphere && !IsUniformScale(info.scale), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #else
    Collider::Collider(EntityHandle handle, ColliderInfo info)
        : ComponentBase(handle),
          m_type{info.type}
    {
    }
    #endif

    ColliderType Collider::GetType() const
    {
        return m_type;
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget(graphics::FrameManager* frame)
    {
        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
            return;

        auto [offset, scale] = collider_detail::GetOffsetAndScaleFromVolume(m_boundingVolume, m_type);

        m_widgetModel.SetTransformationMatrix
        (
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            m_transformMatrix *
            DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
        );

        m_widgetModel.Submit(frame);
    }

    void Collider::EditorGuiElement()
    {
        ImGui::Text("Collider");
        ui::editor::xyzWidgetHeader("     ");
        /** @todo put widgets back */
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
    }
    #endif
}