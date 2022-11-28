#include "physics/Collider.h"
#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/DebugWidget.h"
#include "graphics/Renderer.h"
#include "ncutility/NcError.h"

#ifdef NC_EDITOR_ENABLED
#include "ui/editor/Widgets.h"
#endif

namespace
{
using namespace nc;
using namespace nc::physics;

auto EstimateBoundingVolume(const Sphere& sphere, const Vector3& translation, float scale) -> Sphere;
auto EstimateBoundingVolume(const Box& box, const Vector3& translation, float scale) -> Sphere;
auto EstimateBoundingVolume(const Capsule& capsule, const Vector3& translation, float scale) -> Sphere;
auto EstimateBoundingVolume(const ConvexHull& mesh, const Vector3& translation, float scale) -> Sphere;
auto CreateBoundingVolume(const Collider::VolumeInfo& info) -> BoundingVolume;

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
            throw NcError("Unknown ColliderType");
    }
}
} // anonymous namespace

namespace nc::physics
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
    NC_ASSERT(!HasAnyZeroElement(m_info.scale), "Invalid scale(elements cannot be 0)");
    NC_ASSERT(HasUniformElements(m_info.scale), "Sphere colliders do not support nonuniform scaling");
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
    NC_ASSERT(!HasAnyZeroElement(m_info.scale), "Invalid scale(elements cannot be 0)");
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
    NC_ASSERT(!HasAnyZeroElement(m_info.scale), "Invalid scale(elements cannot be 0)");
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

void Collider::SetProperties(BoxProperties properties)
{
    m_info.type = ColliderType::Box;
    m_info.offset = properties.center;
    m_info.scale = properties.extents;
    m_info.hullAssetPath = "";
    m_volume = CreateBoundingVolume(m_info);
}

void Collider::SetProperties(CapsuleProperties properties)
{
    m_info.type = ColliderType::Capsule;
    m_info.offset = properties.center;
    m_info.scale = Vector3{properties.radius * 2.0f, properties.height * 0.5f, properties.radius * 2.0f};
    m_info.hullAssetPath = "";
    m_volume = CreateBoundingVolume(m_info);
}

void Collider::SetProperties(HullProperties properties)
{
    m_info.type = ColliderType::Hull;
    m_info.offset = Vector3::Zero();
    m_info.scale = Vector3::One();
    m_info.hullAssetPath = std::move(properties.assetPath);
    m_volume = CreateBoundingVolume(m_info);
}

void Collider::SetProperties(SphereProperties properties)
{
    m_info.type = ColliderType::Sphere;
    m_info.offset = properties.center;
    m_info.scale = Vector3::Splat(properties.radius * 2.0f);
    m_info.hullAssetPath = "";
    m_volume = CreateBoundingVolume(m_info);
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
        default: throw NcError("Unknown ColliderType");
    }
}

#ifdef NC_EDITOR_ENABLED
void Collider::SetEditorSelection(bool state)
{
    m_selectedInEditor = state;
}

bool Collider::GetEditorSelection()
{
    return m_selectedInEditor;
}

graphics::DebugWidget Collider::GetDebugWidget()
{
    const auto& scale = m_info.scale;
    const auto& offset = m_info.offset; 

    auto transformationMatrix = DirectX::FXMMATRIX
    (
        DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        ActiveRegistry()->Get<Transform>(ParentEntity())->TransformationMatrix() *
        DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
    );

    return graphics::DebugWidget(m_info.type, transformationMatrix);
}
} // namespace nc::physics

namespace nc
{
template<> void ComponentGuiElement<physics::Collider>(physics::Collider* collider)
{
    const auto& info = collider->GetInfo();

    auto& offset = collider->m_info.offset;

    ImGui::Text("Collider");

    if(ImGui::BeginCombo("Type", ToCString(info.type)))
    {
        if(ImGui::Selectable("Box"))
        {

        }
        if(ImGui::Selectable("Capsule"))
        {

        }
        if(ImGui::Selectable("Hull"))
        {

        }
        if(ImGui::Selectable("Sphere"))
        {
        }
        ImGui::EndCombo();
    }

    ImGui::Checkbox("  ", &collider->m_info.isTrigger);
    ImGui::SameLine();
    ImGui::Text("Trigger");

    switch(collider->m_info.type)
    {
        case ColliderType::Sphere:
        {
            ui::editor::xyzWidgetHeader("   ");
            ui::editor::xyzWidget("Center", "collidercenter", &offset.x, &offset.y, &offset.z, 0.0001f, 1000.0f);

            break;
        }
        default: break;
    }

    /** @todo put widgets back */
}
#endif
} // namespace nc
