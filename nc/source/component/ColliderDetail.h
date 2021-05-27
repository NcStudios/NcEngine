#pragma once

#include "component/Collider.h"

namespace nc::collider_detail
{
    #ifdef NC_EDITOR_ENABLED
    graphics::Model CreateWireframeModel(ColliderType type);
    /** @todo unique_ptr overload only needed for dx11 */
    std::unique_ptr<graphics::Model> CreateWireframeModelPtr(ColliderType type);
    #endif

    Collider::BoundingVolume CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale);
} // namespace nc::collider_detail