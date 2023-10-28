#include "ncengine/graphics/DebugWidget.h"
#include "ncengine/asset/DefaultAssets.h"

#include "ncutility/NcError.h"

namespace
{
using namespace nc::physics;

std::string GetMeshPathForCollider(ColliderType colliderType)
{
    switch(colliderType)
    {
        case ColliderType::Box:
        {
            return nc::asset::CubeMesh;
        }
        case ColliderType::Sphere:
        {
            return nc::asset::SphereMesh;
        }
        case ColliderType::Capsule:
        {
            return nc::asset::CapsuleMesh;
        }
        case ColliderType::Hull:
        {
            // @todo: Eventually not have sphere here.
            return nc::asset::SphereMesh;
        }
        default:
        {
            throw nc::NcError("Unknown ColliderType");
        }
    }
}
} // anonymous namespace

namespace nc::graphics
{
DebugWidget::DebugWidget(physics::ColliderType colliderType, DirectX::FXMMATRIX transform)
    : meshUid{GetMeshPathForCollider(colliderType)},
        transformationMatrix{transform}
{
}
} // namespace nc::graphics
