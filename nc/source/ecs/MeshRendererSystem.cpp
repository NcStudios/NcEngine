#include "MeshRendererSystem.h"
#include "ECS.h"
#include "graphics/Graphics2.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/resources/ResourceManager.h"

namespace nc::ecs
{
    MeshRendererSystem::MeshRendererSystem(registry_type* registry, graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_registry{registry}
    {
        registry->RegisterOnAddCallback<MeshRenderer>
        (
            [this](MeshRenderer& meshRenderer) { this->Add(meshRenderer); }
        );

        registry->RegisterOnRemoveCallback<MeshRenderer>
        (
            [this](Entity entity) { this->Remove(entity); }
        );

        graphics::ResourceManager::InitializeObjects(graphics);
    }

    void MeshRendererSystem::Update()
    {
        const auto& viewMatrix = m_graphics->GetViewMatrix();
        const auto& projectionMatrix = m_graphics->GetProjectionMatrix();
        auto viewProjection = viewMatrix * projectionMatrix;
        auto renderers = m_registry->ViewAll<MeshRenderer>();
        auto objectsData = std::vector<graphics::ObjectData>();
        objectsData.reserve(renderers.size());

        for(const auto& renderer : renderers)
        {
            const auto& matrix = m_registry->Get<Transform>(renderer.GetParentEntity())->GetTransformationMatrix();
            auto [baseIndex, normalIndex, roughnessIndex] = renderer.GetTextureIndices();
            objectsData.emplace_back(matrix, matrix * viewMatrix, viewProjection, baseIndex, normalIndex, roughnessIndex, 1);
        }

        graphics::ResourceManager::UpdateObjects(objectsData);
    }

    void MeshRendererSystem::Add(MeshRenderer&)
    {
    }

    void MeshRendererSystem::Remove(Entity)
    {
    }

    void MeshRendererSystem::Clear()
    {
        graphics::ResourceManager::ResetObjects(m_graphics);
    }
}