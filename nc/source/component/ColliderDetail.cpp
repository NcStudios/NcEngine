#include "ColliderDetail.h"
#include "directx/math/DirectXCollision.h"

namespace
{
    auto ToFloat3(const nc::Vector3& v) { return DirectX::XMFLOAT3{v.x, v.y, v.z}; }

    #ifdef NC_EDITOR_ENABLED
    const auto CubeMeshPath = std::string{"project/assets/mesh/cube.nca"};
    const auto SphereMeshPath = std::string{"project/assets/mesh/sphere.nca"};
    const auto CreateMaterial = nc::graphics::Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>;
    #endif
}

namespace nc::collider_detail
{
    #ifdef NC_EDITOR_ENABLED
    graphics::Model CreateWireframeModel(ColliderType type)
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

    std::unique_ptr<graphics::Model> CreateWireframeModelPtr(ColliderType type)
    {
        switch(type)
        {
            case ColliderType::Box:
                return std::make_unique<graphics::Model>( graphics::Mesh{CubeMeshPath}, CreateMaterial() );
            case ColliderType::Sphere:
                return std::make_unique<graphics::Model>( graphics::Mesh{SphereMeshPath}, CreateMaterial() );
            default:
                throw std::runtime_error("CreateWireFrameModel - Unknown ColliderType");
        }
    }
    #endif

    Collider::BoundingVolume CreateBoundingVolume(ColliderType type, const Vector3& offset, const Vector3& scale)
    {
        switch(type)
        {
            case ColliderType::Box:
                return { DirectX::BoundingOrientedBox{ToFloat3(offset), ToFloat3(scale * 0.5f), {0.0f, 0.0f, 0.0f, 1.0f}} };
            case ColliderType::Sphere:
                return { DirectX::BoundingSphere{ToFloat3(offset), scale.x * 0.5f} };
            default:
                throw std::runtime_error("CreateBoundingVolume - Unknown ColliderType");
        }
    }
} // namespace nc::collider_detail