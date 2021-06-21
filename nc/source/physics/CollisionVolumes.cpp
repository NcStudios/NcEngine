#include "CollisionVolumes.h"
#include "assets/HullColliderManager.h"
#include "physics/IntersectionQueries.h"

namespace nc::physics
{
    SphereCollider EstimateBoundingVolume(const SphereCollider& sphere, const Vector3& translation, float scale);
    SphereCollider EstimateBoundingVolume(const BoxCollider& box, const Vector3& translation, float scale);
    SphereCollider EstimateBoundingVolume(const CapsuleCollider& capsule, const Vector3& translation, float scale);
    SphereCollider EstimateBoundingVolume(const HullCollider& mesh, const Vector3& translation, float scale);
    float GetMatrixScaleExtent(DirectX::FXMMATRIX matrix);

    BoundingVolume CreateBoundingVolume(const Collider::VolumeInfo& info)
    {
        switch(info.type)
        {
            case ColliderType::Box:
            {
                return { BoxCollider{info.offset, info.scale, Magnitude(info.scale / 2.0f)} };
            }
            case ColliderType::Sphere:
            {
                return { SphereCollider{info.offset, info.scale.x * 0.5f} };
            }
            case ColliderType::Capsule:
            {
                auto radius = info.scale.x / 2.0f;
                auto halfSegment = Vector3::Up() * (info.scale.y - radius);
                return { CapsuleCollider{info.offset + halfSegment, info.offset - halfSegment, radius, info.scale.y} };
            }
            case ColliderType::Hull:
            {
                return { HullColliderManager::Acquire(info.meshPath) };
            }
            default:
                throw std::runtime_error("CreateBoundingVolume - Unkown ColliderType");
        }
    }

    SphereCollider EstimateBoundingVolume(const SphereCollider& sphere, const Vector3& translation, float scale)
    {
        return SphereCollider{sphere.center + translation, sphere.radius * scale};
    }

    SphereCollider EstimateBoundingVolume(const BoxCollider& box, const Vector3& translation, float scale)
    {
        return SphereCollider{box.center + translation, box.maxExtent * scale};
    }

    SphereCollider EstimateBoundingVolume(const CapsuleCollider& capsule, const Vector3& translation, float scale)
    {
        return SphereCollider{translation + (capsule.pointA + capsule.pointB) / 2.0f, capsule.maxExtent * scale};
    }

    SphereCollider EstimateBoundingVolume(const HullCollider& mesh, const Vector3& translation, float scale)
    {
        return SphereCollider{translation, mesh.maxExtent * scale};
    }

    SphereCollider EstimateBoundingVolume(const BoundingVolume& volume, DirectX::FXMMATRIX matrix)
    {
        Vector3 translation;
        DirectX::XMStoreVector3(&translation, matrix.r[3]);
        auto scale = GetMatrixScaleExtent(matrix);
        return std::visit([&translation, scale](auto&& v)
        {
            return EstimateBoundingVolume(v, translation, scale);
        }, volume);
    }
    
    float GetMatrixScaleExtent(DirectX::FXMMATRIX matrix)
    {
        auto xExtent_v = DirectX::XMVector3Dot(matrix.r[0], matrix.r[0]);
        auto yExtent_v = DirectX::XMVector3Dot(matrix.r[1], matrix.r[1]);
        auto zExtent_v = DirectX::XMVector3Dot(matrix.r[2], matrix.r[2]);
        auto maxExtent_v = DirectX::XMVectorMax(xExtent_v, DirectX::XMVectorMax(yExtent_v, zExtent_v));
        return sqrt(DirectX::XMVectorGetX(maxExtent_v));
    }
} // namespace nc::physics