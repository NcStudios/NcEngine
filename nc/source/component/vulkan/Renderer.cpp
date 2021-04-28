#include "component/vulkan/Renderer.h"
#include "Ecs.h"

namespace nc::vulkan
{
    Renderer::Renderer(EntityHandle handle)
    : Component{handle}
    {
    }

    Transform* Renderer::GetTransform()
    {
        return GetComponent<Transform>(GetParentHandle());
    }
}