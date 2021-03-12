#pragma once

#include "component/Collider.h"

namespace nc::physics
{
    /** An estimated bounding volume and index mapping to the associated
     *  collider's position in the SoA representation. */
    struct DynamicEstimate
    {
        DirectX::BoundingSphere volumeEstimate;
        uint32_t index;
    };

    struct DynamicActual
    {
        Collider::BoundingVolume volume;
        uint32_t index;
    };

    struct StaticTreeEntry
    {
        Collider::BoundingVolume volume;
        EntityHandle handle;
    };

    /** The collider properties required for constructing bounding volumes. */
    struct VolumeProperties
    {
        DirectX::XMFLOAT3 center;
        DirectX::XMFLOAT3 extents;
    };

    inline VolumeProperties GetVolumePropertiesFromColliderInfo(const ColliderInfo& info)
    {
        return { {info.offset.x, info.offset.y, info.offset.z}, {info.scale.x / 2.0f, info.scale.y / 2.0f, info.scale.z / 2.0f} };
    }

    inline DirectX::BoundingSphere EstimateBoundingVolume(const VolumeProperties& volumeProperties, const DirectX::XMMATRIX* transform)
    {
        const auto& extents = volumeProperties.extents;
        const auto radius = sqrt(extents.x * extents.x + extents.y * extents.y + extents.z * extents.z);
        DirectX::BoundingSphere out{volumeProperties.center, radius};
        out.Transform(out, *transform);
        return out;
    }

    inline nc::Collider::BoundingVolume CalculateBoundingVolume(nc::ColliderType type, const VolumeProperties& volumeProperties, const DirectX::XMMATRIX* transform)
    {
        switch(type)
        {
            case nc::ColliderType::Box:
            {
                DirectX::BoundingOrientedBox out(volumeProperties.center, volumeProperties.extents, {0,0,0,1.0f});
                out.Transform(out, *transform);
                return {out};
            }
            case nc::ColliderType::Sphere:
            {
                DirectX::BoundingSphere out{volumeProperties.center, volumeProperties.extents.x};
                out.Transform(out, *transform);
                return {out};
            }
        }

        throw std::runtime_error("CalculateBoundingVolume - Unknown ColliderType");
    }
}