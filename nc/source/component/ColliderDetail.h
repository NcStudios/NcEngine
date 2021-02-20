#pragma once

#include "component/Collider.h"

namespace nc::collider_detail
{
    #ifdef NC_EDITOR_ENABLED
    graphics::Model CreateWireframeModel(ColliderType type);
    std::pair<Vector3, Vector3> GetOffsetAndScaleFromVolume(const Collider::BoundingVolume& volume, ColliderType type);
    #endif

    Collider::BoundingVolume CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale);
} // namespace nc::collider_detail