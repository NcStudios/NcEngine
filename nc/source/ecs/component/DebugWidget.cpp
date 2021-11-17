#include "ecs/component/DebugWidget.h"
#include "debug/NcError.h"

namespace
{
    using namespace nc;

    const auto CubeMeshPath = std::string{"cube.nca"};
    const auto SphereMeshPath = std::string{"sphere.nca"};
    const auto CapsuleMeshPath = std::string{"capsule.nca"};

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
                throw NcError("Unknown ColliderType");
            }
        }
    }
}

namespace nc
{
    DebugWidget::DebugWidget(ColliderType colliderType, DirectX::FXMMATRIX transform)
        : meshUid{GetMeshPathForCollider(colliderType)},
          transformationMatrix{transform}
    {
    }
}