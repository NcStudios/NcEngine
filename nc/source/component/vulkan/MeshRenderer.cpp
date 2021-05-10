#include "component/vulkan/MeshRenderer.h"
#include "Ecs.h"

namespace nc::vulkan
{
    MeshRenderer::MeshRenderer(EntityHandle handle)
    : Component{handle}
    {
    }

    Transform* MeshRenderer::GetTransform()
    {
        return GetComponent<Transform>(GetParentHandle());
    }
}