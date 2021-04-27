#include "component/vulkan/Renderer.h"
#include "Ecs.h"

namespace nc::vulkan
{
    Renderer::Renderer(EntityHandle handle)
    : Component{handle},
      m_transform{ GetComponent<Transform>(handle) }
    {
    }

    Transform* Renderer::GetTransform()
    {
        return m_transform;
    }
}