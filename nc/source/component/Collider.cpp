#include "component/Collider.h"
#include "Ecs.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
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
    const auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>;

    #ifdef NC_DEBUG_BUILD
    bool IsUniformScale(const nc::Vector3& scale)
    {
        return nc::math::FloatEqual(scale.x, scale.y) && nc::math::FloatEqual(scale.y, scale.z);
    }
    #endif

    // Changed to unique_ptr for dx11, change back with vulkan
    // nc::graphics::Model CreateWireframeModel(nc::ColliderType type)
    // {
    //     switch(type)
    //     {
    //         case nc::ColliderType::Box:
    //             return nc::graphics::Model{ {CubeMeshPath}, CreateMaterial() };
    //         case nc::ColliderType::Sphere:
    //             return nc::graphics::Model{ {SphereMeshPath}, CreateMaterial() };
    //         default:
    //             throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
    //     }
    // }

    /** @todo Currently no solution for hull widget. */
    std::unique_ptr<nc::graphics::Model> CreateWireframeModelPtr(nc::ColliderType type)
    {
        const std::string& path = [](nc::ColliderType type) -> const std::string&
        {
            switch(type)
            {
                case nc::ColliderType::Box: return CubeMeshPath;
                case nc::ColliderType::Sphere: return SphereMeshPath;
                case nc::ColliderType::Capsule: return CapsuleMeshPath;
                case nc::ColliderType::Hull: return SphereMeshPath;
                default: throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
            }
        }(type);

        return std::make_unique<nc::graphics::Model>(nc::graphics::Mesh{path}, CreateMaterial());
    }
}
#endif

namespace nc
{
    Collider::Collider(Entity entity, SphereProperties properties, bool isTrigger)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Sphere,
                 .offset = properties.center,
                 .scale = Vector3::Splat(properties.radius * 2.0f),
                 .assetPath = "",
                 .isTrigger = isTrigger}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_widgetModel{CreateWireframeModelPtr(ColliderType::Sphere)},
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(!IsUniformScale(m_info.scale), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }

    Collider::Collider(Entity entity, BoxProperties properties, bool isTrigger)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Box,
                 .offset = properties.center,
                 .scale = properties.extents,
                 .assetPath = "",
                 .isTrigger = isTrigger}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_widgetModel{CreateWireframeModelPtr(ColliderType::Box)},
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
    }

    Collider::Collider(Entity entity, CapsuleProperties properties, bool isTrigger)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Capsule,
                 .offset = properties.center,
                 .scale = Vector3{properties.radius * 2.0f, properties.height / 2.0f, properties.radius * 2.0f},
                 .assetPath = "",
                 .isTrigger = isTrigger}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_widgetModel{CreateWireframeModelPtr(ColliderType::Capsule)},
          m_selectedInEditor{false}
          #endif
    {
        IF_THROW(HasAnyZeroElement(m_info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
    }

    Collider::Collider(Entity entity, HullProperties properties, bool isTrigger)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Hull,
                 .offset = Vector3::Zero(),
                 .scale = Vector3::One(),
                 .assetPath = std::move(properties.assetPath),
                 .isTrigger = isTrigger}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_widgetModel{CreateWireframeModelPtr(ColliderType::Sphere)},
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
    void Collider::UpdateWidget(graphics::FrameManager* frame)
    {
        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
            return;

        const auto& scale = m_info.scale;
        const auto& offset = m_info.offset; 

        m_widgetModel->SetTransformationMatrix
        (
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            ActiveRegistry()->Get<Transform>(GetParentEntity())->GetTransformationMatrix() *
            DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
        );

        m_widgetModel->Submit(frame);
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
    }

    const char* ToCString(nc::ColliderType type)
    {
        switch(type)
        {
            case nc::ColliderType::Box:     return "Box";
            case nc::ColliderType::Capsule: return "Capsule";
            case nc::ColliderType::Sphere:  return "Sphere";
            case nc::ColliderType::Hull:    return "Hull";
            default: throw std::runtime_error("ToCString - Unknown ColliderType");
        }
    }

    template<> void ComponentGuiElement<Collider>(Collider* collider)
    {
        const auto& info = collider->GetInfo();
        ImGui::Text("Collider");
        ImGui::Text("  Type:    %s", ToCString(info.type));
        ImGui::Text("  Trigger: %s", info.isTrigger ? "True" : "False");
        /** @todo put widgets back */
    }
    #endif
}