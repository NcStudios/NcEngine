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
    Collider::Collider(EntityHandle handle, ColliderType type, Vector3 offset, Vector3 scale)
        : ComponentBase(handle),
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_boundingVolume{collider_detail::CreateBoundingVolume(type, offset, scale)},
          m_type{type},
          m_widgetModel{collider_detail::CreateWireframeModel(type)},
          m_selectedInEditor{false}
    {
        IF_THROW(!HasNoZeroElement(scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(type == ColliderType::Sphere && scale - scale != Vector3::Zero(), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #else
    Collider::Collider(EntityHandle handle, ColliderType type, Vector3 offset, Vector3 scale)
        : ComponentBase(handle),
          m_transformMatrix{GetComponent<Transform>(handle)->GetTransformationMatrix()},
          m_boundingVolume{collider_detail::CreateBoundingVolume(type, offset, scale)},
          m_type{type}
    {
        IF_THROW(!HasNoZeroElement(scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(type == ColliderType::Sphere && scale - scale != Vector3::Zero(), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #endif

    Collider::~Collider() = default;

    ColliderType Collider::GetType() const
    {
        return m_type;
    }

    Collider::BoundingVolume Collider::CalculateBoundingVolume() const
    {
        return std::visit([this](auto&& volume) 
        {
            using Volume_t = std::remove_cvref<decltype(volume)>::type;
            Volume_t out;
            volume.Transform(out, m_transformMatrix);
            return BoundingVolume{out};
        }, m_boundingVolume);
    }

    DirectX::BoundingSphere Collider::EstimateBoundingVolume() const
    {
        /** possible optimization: In narrow phase CalculateBoundingVolume will run this same code(for spheres).
         *  Could maybe save the volume - would be tricky though as it only works for spheres. */
        auto out = [this]()
        {
            if(m_type == ColliderType::Sphere)
            {
                return std::get<DirectX::BoundingSphere>(m_boundingVolume);
            }
            else
            {
                DirectX::BoundingSphere sphere;
                DirectX::BoundingSphere::CreateFromBoundingBox(sphere, std::get<DirectX::BoundingOrientedBox>(m_boundingVolume));
                return sphere;
            }
        }();

        out.Transform(out, m_transformMatrix);
        return out;
    }


    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget(graphics::FrameManager& frame)
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