#include "graphics/SkeletalAnimator.h"

#include "graphics/MeshRenderer.h"
#include "graphics/ToonRenderer.h"
#include "ecs/Registry.h"

#include <functional>

namespace
{
auto GetMeshPath(nc::Entity entity) -> std::string
{
    auto* registry = nc::ActiveRegistry();
    auto* meshRenderer = registry->Get<nc::graphics::MeshRenderer>(entity);
    auto* toonRenderer = registry->Get<nc::graphics::ToonRenderer>(entity);
    if (meshRenderer)
    {
        return meshRenderer->GetMeshPath();
    }
    else if (toonRenderer)
    {
        return toonRenderer->GetMeshPath();
    }
    else
    {
        throw nc::NcError("SkeletalAnimator must be added to an Entity with either a ToonRenderer or a MeshRenderer.");
    }
}
}

namespace nc::graphics
{

} // namespace nc::graphics