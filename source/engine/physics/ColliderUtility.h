#pragma once

#include "ncengine/physics/Collider.h"
#include "assets/AssetService.h"

namespace nc::physics
{
inline auto ToVolumeInfo(const BoxProperties& in) noexcept -> VolumeInfo
{
    return VolumeInfo{ColliderType::Box, in.center, in.extents};
}

inline auto ToVolumeInfo(const SphereProperties& in) noexcept -> VolumeInfo
{
    return VolumeInfo{ColliderType::Sphere, in.center, Vector3::Splat(in.radius * 2.0f )};
}

inline auto ToVolumeInfo(const CapsuleProperties& in) noexcept -> VolumeInfo
{
    const auto scale = Vector3{in.radius * 2.0f, in.height * 0.5f, in.radius * 2.0f};
    return VolumeInfo{ColliderType::Capsule, in.center, scale};
}

inline auto ToVolumeInfo(const HullProperties&) noexcept -> VolumeInfo
{
    return VolumeInfo{ColliderType::Hull, Vector3::Zero(), Vector3::One()};
}

inline auto ToBoxProperties(const VolumeInfo& in) noexcept -> BoxProperties
{
    return BoxProperties{in.offset, in.scale};
}

inline auto ToSphereProperties(const VolumeInfo& in) noexcept -> SphereProperties
{
    return SphereProperties{in.offset, in.scale.x * 0.5f};
}

inline auto ToCapsuleProperties(const VolumeInfo& in) noexcept -> CapsuleProperties
{
    return CapsuleProperties{in.offset, in.scale.y * 2.0f, in.scale.x * 0.5f};
}

inline auto MakeBoundingVolume(const BoxProperties& in) -> BoundingVolume
{
    return { Box{in.center, in.extents, Magnitude(in.extents / 2.0f)} };
}

inline auto MakeBoundingVolume(const SphereProperties& in) -> BoundingVolume
{
    return { Sphere{in.center, in.radius} };
}

inline auto MakeBoundingVolume(const CapsuleProperties& in) -> BoundingVolume
{
    const auto halfHeight = in.height * 0.5f;
    const auto pointOffset = Vector3::Up() * (halfHeight - in.radius);
    const auto a = in.center + pointOffset;
    const auto b = in.center - pointOffset;
    return { Capsule{a, b, in.radius, halfHeight} };
}

inline auto MakeBoundingVolume(const HullProperties& in) -> BoundingVolume
{
    auto hull = asset::AssetService<asset::ConvexHullView>::Get()->Acquire(in.assetPath);
    return { ConvexHull{hull.vertices, hull.extents, hull.maxExtent} };
}

inline auto MakeBoundingSphere(const Sphere& sphere, const Vector3& translation, float scale) noexcept -> Sphere
{
    return Sphere{sphere.center + translation, sphere.radius * scale};
}

inline auto MakeBoundingSphere(const Box& box, const Vector3& translation, float scale) noexcept -> Sphere
{
    return Sphere{box.center + translation, box.maxExtent * scale};
}

inline auto MakeBoundingSphere(const Capsule& capsule, const Vector3& translation, float scale) noexcept -> Sphere
{
    return Sphere{translation + (capsule.pointA + capsule.pointB) / 2.0f, capsule.maxExtent * scale};
}

inline auto MakeBoundingSphere(const ConvexHull& mesh, const Vector3& translation, float scale) noexcept -> Sphere
{
    return Sphere{translation, mesh.maxExtent * scale};
}
} // namespace nc::physics
