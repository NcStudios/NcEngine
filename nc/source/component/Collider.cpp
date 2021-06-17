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
    const auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>;

    bool IsUniformScale(const nc::Vector3& scale)
    {
        return nc::math::FloatEqual(scale.x, scale.y) && nc::math::FloatEqual(scale.y, scale.z);
    }
    
    nc::graphics::Model CreateWireframeModel(nc::ColliderType type)
    {
        switch(type)
        {
            case nc::ColliderType::Box:
                return nc::graphics::Model{ {CubeMeshPath}, CreateMaterial() };
            case nc::ColliderType::Sphere:
                return nc::graphics::Model{ {SphereMeshPath}, CreateMaterial() };
            default:
                throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
        }
    }

    std::unique_ptr<nc::graphics::Model> CreateWireframeModelPtr(nc::ColliderType type)
    {
        switch(type)
        {
            case nc::ColliderType::Box:
                return std::make_unique<nc::graphics::Model>( nc::graphics::Mesh{CubeMeshPath}, CreateMaterial() );
            case nc::ColliderType::Sphere:
                return std::make_unique<nc::graphics::Model>( nc::graphics::Mesh{SphereMeshPath}, CreateMaterial() );
            default:
                throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
        }
    }
}
#endif

namespace nc
{
    Collider::BoundingVolume CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale)
    {
        switch(type)
        {
            //case ColliderType::Box:
            //    return { BoxCollider{offset, scale * 0.5f} };
            case ColliderType::Box:
                return { BoxCollider{offset, scale} };
            case ColliderType::Sphere:
                return { SphereCollider{offset, scale.x * 0.5f} };
            case ColliderType::Mesh:
                return { MeshCollider{} };
            default:
                throw std::runtime_error("CreateBoundingVolume - Unkown ColliderType");
        }
    }

    #ifdef NC_EDITOR_ENABLED
    Collider::Collider(Entity entity, ColliderInfo info)
        : ComponentBase(entity),
          m_info{info},
          m_widgetModel{CreateWireframeModelPtr(info.type)},
          m_selectedInEditor{false}
    {
        IF_THROW(HasAnyZeroElement(info.scale), "Collider::Collider - Invalid scale(elements cannot be 0)");
        IF_THROW(info.type == ColliderType::Sphere && !IsUniformScale(info.scale), "Collider::Collider - Sphere colliders do not support nonuniform scaling");
    }
    #else
    Collider::Collider(Entity entity, ColliderInfo info)
        : ComponentBase(entity),
          m_info{info}
    {
    }
    #endif

    const ColliderInfo& Collider::GetInfo() const
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

    template<> void ComponentGuiElement<Collider>(Collider*)
    {
        ImGui::Text("Collider");
        /** @todo put widgets back */
    }
    #endif
}