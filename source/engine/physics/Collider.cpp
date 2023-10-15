#include "physics/Collider.h"
#include "ColliderUtility.h"
#include "assets/AssetService.h"
#include "ecs/Registry.h"
#include "graphics/DebugWidget.h"

#include "ncutility/NcError.h"

namespace
{
} // anonymous namespace

namespace nc::physics
{
Collider::Collider(Entity entity, SphereProperties properties, bool isTrigger)
    : ComponentBase(entity),
      m_volume{MakeBoundingVolume(properties)},
      m_isTrigger{isTrigger},
      m_awake{true},
      m_coldData{std::make_unique<ColliderColdData>(ToVolumeInfo(properties))}
{
    NC_ASSERT(!HasAnyZeroElement(m_coldData->info.scale), "Invalid scale(elements cannot be 0)");
    NC_ASSERT(HasUniformElements(m_coldData->info.scale), "Sphere colliders do not support nonuniform scaling");
}

Collider::Collider(Entity entity, BoxProperties properties, bool isTrigger)
    : ComponentBase(entity),
      m_volume{MakeBoundingVolume(properties)},
      m_isTrigger{isTrigger},
      m_awake{true},
      m_coldData{std::make_unique<ColliderColdData>(ToVolumeInfo(properties))}
{
    NC_ASSERT(!HasAnyZeroElement(m_coldData->info.scale), "Invalid scale(elements cannot be 0)");
}

Collider::Collider(Entity entity, CapsuleProperties properties, bool isTrigger)
    : ComponentBase(entity),
      m_volume{MakeBoundingVolume(properties)},
      m_isTrigger{isTrigger},
      m_awake{true},
      m_coldData{std::make_unique<ColliderColdData>(ToVolumeInfo(properties))}
{
    NC_ASSERT(!HasAnyZeroElement(m_coldData->info.scale), "Invalid scale(elements cannot be 0)");
}

Collider::Collider(Entity entity, HullProperties properties, bool isTrigger)
    : ComponentBase(entity),
      m_volume{MakeBoundingVolume(properties)},
      m_isTrigger{isTrigger},
      m_awake{true},
      m_coldData{std::make_unique<ColliderColdData>(ToVolumeInfo(properties), std::move(properties.assetPath))}
{
}

void Collider::SetProperties(BoxProperties properties)
{
    m_volume = MakeBoundingVolume(properties);
    m_coldData->info = ToVolumeInfo(properties);
    m_coldData->assetPath = "";
}

void Collider::SetProperties(CapsuleProperties properties)
{
    m_volume = MakeBoundingVolume(properties);
    m_coldData->info = ToVolumeInfo(properties);
    m_coldData->assetPath = "";
}

void Collider::SetProperties(HullProperties properties)
{
    m_volume = MakeBoundingVolume(properties);
    m_coldData->info = ToVolumeInfo(properties);
    m_coldData->assetPath = std::move(properties.assetPath);
}

void Collider::SetProperties(SphereProperties properties)
{
    m_volume = MakeBoundingVolume(properties);
    m_coldData->info = ToVolumeInfo(properties);
    m_coldData->assetPath = "";
}

auto Collider::EstimateBoundingVolume(DirectX::FXMMATRIX matrix) const -> Sphere
{
    Vector3 translation;
    DirectX::XMStoreVector3(&translation, matrix.r[3]);
    auto scale = GetMaxScaleExtent(matrix);
    return std::visit([&translation, scale](auto&& v)
    {
        return MakeBoundingSphere(v, translation, scale);
    }, m_volume);
}

auto ToString(ColliderType type) -> std::string_view
{
    using namespace std::string_view_literals;
    switch(type)
    {
        case ColliderType::Box:     return "Box"sv;
        case ColliderType::Capsule: return "Capsule"sv;
        case ColliderType::Sphere:  return "Sphere"sv;
        case ColliderType::Hull:    return "Hull"sv;
        default: throw NcError("Unknown ColliderType");
    }
}

auto FromString(std::string_view type) -> ColliderType
{
    using namespace std::string_view_literals;
    if      (type == "Box"sv)     return ColliderType::Box;
    else if (type == "Capsule"sv) return ColliderType::Capsule;
    else if (type == "Hull"sv)    return ColliderType::Hull;
    else if (type == "Sphere"sv)  return ColliderType::Sphere;
    throw NcError{fmt::format("Failed to parse ColliderType: {}", type)};
}

#ifdef NC_EDITOR_ENABLED
void Collider::SetEditorSelection(bool state)
{
    m_coldData->selectedInEditor = state;
}

bool Collider::GetEditorSelection()
{
    return m_coldData->selectedInEditor;
}

graphics::DebugWidget Collider::GetDebugWidget()
{
    const auto& scale = m_coldData->info.scale;
    const auto& offset = m_coldData->info.offset;

    auto transformationMatrix = DirectX::FXMMATRIX
    (
        DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
        ActiveRegistry()->Get<Transform>(ParentEntity())->TransformationMatrix() *
        DirectX::XMMatrixTranslation(offset.x, offset.y, offset.z)
    );

    return graphics::DebugWidget(m_coldData->info.type, transformationMatrix);
}
#endif
} // namespace nc::physics
