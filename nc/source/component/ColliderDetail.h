#pragma once

#include "component/Collider.h"
#include "directx/math/DirectXCollision.h"

namespace nc::collider_detail
{
    inline auto ToVec3(const DirectX::XMFLOAT3& v) { return Vector3{v.x, v.y, v.z}; }
    inline auto ToFloat3(const Vector3& v) { return DirectX::XMFLOAT3{v.x, v.y, v.z}; }

    #ifdef NC_EDITOR_ENABLED
    const auto CubeMeshPath = std::string{"nc/resources/mesh/cube.fbx"};
    const auto SphereMeshPath = std::string{"nc/resources/mesh/sphere.fbx"};
    auto CreateMaterial = graphics::Material::CreateMaterial<graphics::TechniqueType::Wireframe>;

    inline auto CreateWireframeModel(ColliderType type)
    {
        switch(type)
        {
            case ColliderType::Box:
                return graphics::Model{ {CubeMeshPath}, CreateMaterial() };
            case ColliderType::Sphere:
                return graphics::Model{ {SphereMeshPath}, CreateMaterial() };
            default:
                throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
        }
    }

    inline auto GetOffsetAndScaleFromVolume(const Collider::BoundingVolume& volume, ColliderType type)
    {
        switch(type)
        {
            case ColliderType::Box:
            {
                auto box = std::get<DirectX::BoundingOrientedBox>(volume);
                return std::pair{ToVec3(box.Center), ToVec3(box.Extents) * 2.0f};
            }
            case ColliderType::Sphere:
            {
                auto sphere = std::get<DirectX::BoundingSphere>(volume);
                return std::pair{ToVec3(sphere.Center), Vector3::Splat(sphere.Radius) * 2.0f};
            }
        }

        throw std::runtime_error("GetOffsetAndScaleFromVolume - Unknown ColliderType");
    }
    #endif

    inline auto CreateBoundingBox(const Vector3& offset, const Vector3& extents)
    {
        return Collider::BoundingVolume(DirectX::BoundingOrientedBox{ToFloat3(offset), ToFloat3(extents), {0.0f, 0.0f, 0.0f, 1.0f}});
    }

    inline auto CreateBoundingSphere(const Vector3& offset, float radius)
    {
        return Collider::BoundingVolume(DirectX::BoundingSphere{ToFloat3(offset), radius});
    }

    inline auto CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale)
    {
        switch(type)
        {
            case ColliderType::Box:
                return CreateBoundingBox(offset, scale * 0.5f);
            case ColliderType::Sphere:
                return CreateBoundingSphere(offset, scale.x * 0.5f);
            default:
                throw std::runtime_error("CreateBoundingVolume - Unknown ColliderType");
        }
    }
} // namespace nc::collider_detail