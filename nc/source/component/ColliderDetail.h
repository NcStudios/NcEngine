#pragma once

#include "component/Collider.h"
#include "directx/math/DirectXCollision.h"

namespace nc::collider_detail
{
    #ifdef NC_EDITOR_ENABLED
    const auto CubeMeshPath = std::string{"nc/resources/mesh/cube.fbx"};
    const auto SphereMeshPath = std::string{"nc/resources/mesh/sphere.fbx"};
    auto CreateMaterial = graphics::Material::CreateMaterial<graphics::TechniqueType::Wireframe>;

    auto CreateWireframeModel(ColliderType type)
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
    #endif

    inline auto CreateBoundingBox(const Vector3& offset, const Vector3& scale)
    {
        return std::make_unique<Collider::BoundingVolume>
        (
            DirectX::BoundingOrientedBox
            {
                {offset.x, offset.y, offset.z},
                {scale.x, scale.y, scale.z},
                {0.0f, 0.0f, 0.0f, 1.0f}
            }
        );
    }

    inline auto CreateBoundingSphere(const Vector3& offset, const Vector3& scale)
    {
        return std::make_unique<Collider::BoundingVolume>
        (
            DirectX::BoundingSphere{{offset.x, offset.y, offset.z}, scale.x}
        );
    }

    inline auto CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale)
    {
        switch(type)
        {
            case ColliderType::Box:
                return CreateBoundingBox(offset, scale);
            case ColliderType::Sphere:
                return CreateBoundingSphere(offset, scale);
            default:
                throw std::runtime_error("CreateBoundingVolume - Unknown ColliderType");
        }
    }
} // namespace nc::collider_detail