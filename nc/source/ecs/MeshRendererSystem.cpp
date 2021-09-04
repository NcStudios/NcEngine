#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Material.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/resources/ResourceManager.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_registry{registry}
    {
        registry->RegisterOnAddCallback<vulkan::MeshRenderer>
        (
            [this](vulkan::MeshRenderer& meshRenderer) { this->Add(meshRenderer); }
        );

        registry->RegisterOnRemoveCallback<vulkan::MeshRenderer>
        (
            [this](Entity entity) { this->Remove(entity); }
        );

        graphics::vulkan::ResourceManager::InitializeObjects(graphics);
    }

    void MeshRendererSystem::Update()
    {
        const auto& viewMatrix = m_graphics->GetViewMatrix();
        const auto& projectionMatrix = m_graphics->GetProjectionMatrix();

        auto meshRenderers = m_registry->ViewAll<vulkan::MeshRenderer>();
        for (auto& meshRenderer : meshRenderers)
        {
            meshRenderer.Update(m_registry->Get<Transform>(meshRenderer.GetParentEntity()), viewMatrix, projectionMatrix);
        }

        auto objectsData = std::vector<vulkan::ObjectData>();
        objectsData.reserve(meshRenderers.size());

        std::transform(meshRenderers.begin(), meshRenderers.end(), std::back_inserter(objectsData), [](auto&& meshRenderer)
        { 
            return meshRenderer.GetObjectData(); 
        });

        graphics::vulkan::ResourceManager::UpdateObjects(objectsData);
    }

    void MeshRendererSystem::Add(vulkan::MeshRenderer&)
    {
    }

    void MeshRendererSystem::Remove(Entity)
    {
    }

    void MeshRendererSystem::Clear()
    {
        graphics::vulkan::ResourceManager::ResetObjects(m_graphics);
    }
}