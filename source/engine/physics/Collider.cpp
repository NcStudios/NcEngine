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
