#include "component/Collider.h"
#include "component/vulkan/DebugWidget.h"
#include "Ecs.h"
#include "debug/Utils.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

#ifdef NC_EDITOR_ENABLED
namespace
{
    const auto CubeMeshPath = std::string{"project/assets/mesh/cube.nca"};
    const auto SphereMeshPath = std::string{"project/assets/mesh/sphere.nca"};
    const auto CapsuleMeshPath = std::string{"project/assets/mesh/capsule.nca"};

    #ifdef NC_DEBUG_BUILD
    bool IsUniformScale(const nc::Vector3& scale)
    {
        return nc::math::FloatEqual(scale.x, scale.y) && nc::math::FloatEqual(scale.y, scale.z);
    }
    #endif
}
#endif

namespace nc
{
    Collider::Collider(Entity entity, SphereProperties properties)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Sphere,
                 .offset = properties.center,
                 .scale = Vector3::Splat(properties.radius * 2.0f),
                 .assetPath = ""}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(!IsUniformScale(m_info.scale), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }

    Collider::Collider(Entity entity, BoxProperties properties)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Box,
                 .offset = properties.center,
                 .scale = properties.extents,
                 .assetPath = ""}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
    }

    Collider::Collider(Entity entity, CapsuleProperties properties)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Capsule,
                 .offset = properties.center,
                 .scale = Vector3{properties.radius * 2.0f, properties.height / 2.0f, properties.radius * 2.0f},
                 .assetPath = ""}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
    }

    Collider::Collider(Entity entity, HullProperties properties)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Hull,
                 .offset = Vector3::Zero(),
                 .scale = Vector3::One(),
                 .assetPath = std::move(properties.assetPath)}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_selectedInEditor{false}
          #endif
    {
    }

    const Collider::VolumeInfo& Collider::GetInfo() const
    {
        return m_info;
    }

    ColliderType Collider::GetType() const
    {
        return m_info.type;
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget()
    {
        auto* debugWidget = ActiveRegistry()->Get<vulkan::DebugWidget>(GetParentEntity());

        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
        {
            debugWidget->Enable(false);
            return;
        }

        debugWidget->Enable(true);

        const auto& scale = m_info.scale;
        const auto& offset = m_info.offset; 

        debugWidget->SetTransformationMatrix
        (
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            ActiveRegistry()->Get<Transform>(GetParentEntity())->GetTransformationMatrix() *
            DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
        );
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
    }

    template<> void ComponentGuiElement<Collider>(Collider*)
    {
        ImGui::Text("Collider");
        /** @todo put widgets back */
    }
    #endif
}