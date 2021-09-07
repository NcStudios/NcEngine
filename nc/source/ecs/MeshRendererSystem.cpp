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

    void MeshRendererSystem::Update()
    {
        const auto& viewMatrix = m_graphics->GetViewMatrix();
        const auto& projectionMatrix = m_graphics->GetProjectionMatrix();

        auto meshRenderers = m_registry->ViewAll<MeshRenderer>();
        for (auto& meshRenderer : meshRenderers)
        {
            meshRenderer.Update(m_registry->Get<Transform>(meshRenderer.GetParentEntity()), viewMatrix, projectionMatrix);
        }

        auto objectsData = std::vector<ObjectData>();
        objectsData.reserve(meshRenderers.size());

        std::transform(meshRenderers.begin(), meshRenderers.end(), std::back_inserter(objectsData), [](auto&& meshRenderer)
        { 
            return meshRenderer.GetObjectData(); 
        });

        graphics::ResourceManager::UpdateObjects(objectsData);
    }

    void MeshRendererSystem::Clear()
    {
        graphics::ResourceManager::ResetObjects(m_graphics);
    }
}