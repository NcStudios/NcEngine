#include "component/vulkan/DebugWidget.h"
#include <stdexcept>

namespace
{
    using namespace nc;
    using namespace nc::vulkan;

    const auto CubeMeshPath = std::string{"project/assets/mesh/cube.nca"};
    const auto SphereMeshPath = std::string{"project/assets/mesh/sphere.nca"};
    const auto CapsuleMeshPath = std::string{"project/assets/mesh/capsule.nca"};

    std::string GetMeshPathForCollider(ColliderType colliderType)
    {
        switch(colliderType)
        {
            case ColliderType::Box:
            {
                return CubeMeshPath;
            }
            case ColliderType::Sphere:
            {
                return SphereMeshPath;
            }
            case ColliderType::Capsule:
            {
                return CapsuleMeshPath;
            }
            case ColliderType::Hull:
            {
                // @todo: Eventually not have sphere here.
                return SphereMeshPath;
            }
            default:
            {
                throw std::runtime_error("DebugWidget::GetMeshPathForCollider - The given colliderType was not a valid ColliderType.");
            }
        }
    }
}

namespace nc::vulkan
{
    DebugWidget::DebugWidget(ColliderType colliderType, DirectX::FXMMATRIX transform)
        : meshUid{GetMeshPathForCollider(colliderType)},
          transformationMatrix{transform}
    {
    }
}