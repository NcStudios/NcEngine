#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/resources/ResourceManager.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(registry_type* registry, graphics::Graphics* graphics)
    : m_graphics{graphics},
      m_registry{registry}
    {
        graphics::ResourceManager::InitializeObjects(graphics);
    }

    void MeshRendererSystem::Clear()
    {
        graphics::ResourceManager::ResetObjects(m_graphics);
    }
}