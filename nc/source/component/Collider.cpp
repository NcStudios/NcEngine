#include "component/Collider.h"
#include "component/DebugWidget.h"
#include "graphics/Renderer.h"
#include "Ecs.h"
#include "debug/Utils.h"
#include "assets/AssetService.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
    using namespace nc;

    auto EstimateBoundingVolume(const Sphere& sphere, const Vector3& translation, float scale) -> Sphere;
    auto EstimateBoundingVolume(const Box& box, const Vector3& translation, float scale) -> Sphere;
    auto EstimateBoundingVolume(const Capsule& capsule, const Vector3& translation, float scale) -> Sphere;
    auto EstimateBoundingVolume(const ConvexHull& mesh, const Vector3& translation, float scale) -> Sphere;
    auto CreateBoundingVolume(const Collider::VolumeInfo& info) -> BoundingVolume;

    #ifdef NC_DEBUG_BUILD
    bool IsUniformScale(const Vector3& scale)
    {
        return math::FloatEqual(scale.x, scale.y) && math::FloatEqual(scale.y, scale.z);
    }
    #endif

    Sphere EstimateBoundingVolume(const Sphere& sphere, const Vector3& translation, float scale)
    {
        return Sphere{sphere.center + translation, sphere.radius * scale};
    }

    Sphere EstimateBoundingVolume(const Box& box, const Vector3& translation, float scale)
    {
        return Sphere{box.center + translation, box.maxExtent * scale};
    }

    Sphere EstimateBoundingVolume(const Capsule& capsule, const Vector3& translation, float scale)
    {
        return Sphere{translation + (capsule.pointA + capsule.pointB) / 2.0f, capsule.maxExtent * scale};
    }

    Sphere EstimateBoundingVolume(const ConvexHull& mesh, const Vector3& translation, float scale)
    {
        return Sphere{translation, mesh.maxExtent * scale};
    }

    BoundingVolume CreateBoundingVolume(const Collider::VolumeInfo& info)
    {
        switch(info.type)
        {
            case ColliderType::Box:
            {
                return { Box{info.offset, info.scale, Magnitude(info.scale / 2.0f)} };
            }
            case ColliderType::Sphere:
            {
                return { Sphere{info.offset, info.scale.x / 2.0f} };
            }
            case ColliderType::Capsule:
            {
                auto radius = info.scale.x / 2.0f;
                auto halfSegment = Vector3::Up() * (info.scale.y - radius);
                return { Capsule{info.offset + halfSegment, info.offset - halfSegment, radius, info.scale.y} };
            }
            case ColliderType::Hull:
            {
                auto hull = AssetService<ConvexHullView>::Get()->Acquire(info.hullAssetPath);
                return { ConvexHull{.vertices = hull.vertices, .extents = hull.extents, .maxExtent = hull.maxExtent} };
            }
            default:
                throw std::runtime_error("CreateBoundingVolume - Unknown ColliderType");
        }
    }
}

namespace nc
{
    Collider::Collider(Entity entity, SphereProperties properties, bool isTrigger)
        : ComponentBase(entity),
          m_info{.type = ColliderType::Sphere,
                 .offset = properties.center,
                 .scale = Vector3::Splat(properties.radius * 2.0f),
                 .hullAssetPath = "",
                 .isTrigger = isTrigger},
          m_volume{CreateBoundingVolume(m_info)},
          m_awake{true}
          #ifdef NC_EDITOR_ENABLED
          ,
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
                 .hullAssetPath = "",
                 .isTrigger = isTrigger},
          m_volume{CreateBoundingVolume(m_info)},
          m_awake{true}
          #ifdef NC_EDITOR_ENABLED
          ,
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
                 .hullAssetPath = "",
                 .isTrigger = isTrigger},
          m_volume{CreateBoundingVolume(m_info)},
          m_awake{true}
          #ifdef NC_EDITOR_ENABLED
          ,
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
                 .hullAssetPath = std::move(properties.assetPath),
                 .isTrigger = isTrigger},
          m_volume{CreateBoundingVolume(m_info)},
          m_awake{true}
          #ifdef NC_EDITOR_ENABLED
          ,
          m_selectedInEditor{false}
          #endif
    {
    }
    
    auto Collider::EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> Sphere
    {
        Vector3 translation;
        DirectX::XMStoreVector3(&translation, matrix.r[3]);
        auto scale = GetMaxScaleExtent(matrix);
        return std::visit([&translation, scale](auto&& v)
        {
            return ::EstimateBoundingVolume(v, translation, scale);
        }, m_volume);
    }

    const char* ToCString(ColliderType type)
    {
        switch(type)
        {
            case ColliderType::Box:     return "Box";
            case ColliderType::Capsule: return "Capsule";
            case ColliderType::Sphere:  return "Sphere";
            case ColliderType::Hull:    return "Hull";
            default: throw std::runtime_error("ToCString - Unknown ColliderType");
        }
    }

    #ifdef NC_EDITOR_ENABLED
    void Collider::UpdateWidget(graphics::Renderer* renderer)
    {
        // Expire to false to avoid state management in editor (it sets this to true as needed)
        if(!std::exchange(m_selectedInEditor, false))
        {
            return;
        }

        const auto& scale = m_info.scale;
        const auto& offset = m_info.offset; 

        auto transformationMatrix = DirectX::FXMMATRIX
        (
            DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
            ActiveRegistry()->Get<Transform>(GetParentEntity())->GetTransformationMatrix() *
            DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
        );

        renderer->RegisterDebugWidget(DebugWidget(m_info.type, transformationMatrix));
    }

    void Collider::SetEditorSelection(bool state)
    {
        m_selectedInEditor = state;
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