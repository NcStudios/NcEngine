#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/techniques/SimpleTechnique.h"
#include "component/Transform.h"

namespace nc::graphics::vulkan
{
    TechniqueManager::TechniqueManager()
    : m_techniques{},
      m_globalData{}
    {
    }

    void TechniqueManager::RegisterGlobalData(vk::Buffer* vertexBuffer, vk::Buffer* indexBuffer)
    {
        m_globalData.vertexBuffer = vertexBuffer;
        m_globalData.indexBuffer = indexBuffer;
    }

    void TechniqueManager::RegisterRenderer(TechniqueType technique, Mesh mesh, Transform* transform)
    {
        for (auto& registeredTechnique : m_techniques)
        {
            if (registeredTechnique->GetType() == technique)
            {
                registeredTechnique->RegisterMesh(mesh);
                registeredTechnique->RegisterTransform(mesh.uid, transform);
                return;
            }
        }

        auto techniqueToRegister = CreateTechnique(technique);
        techniqueToRegister->RegisterMesh(mesh);
        techniqueToRegister->RegisterTransform(mesh.uid, transform);
        m_techniques.push_back(std::move(techniqueToRegister));
    }

    void TechniqueManager::RecordTechniques(Commands* commands)
    {
        for (auto& registeredTechnique : m_techniques)
        {
            registeredTechnique->Record(commands);
        }
    }

    std::unique_ptr<TechniqueBase> TechniqueManager::CreateTechnique(TechniqueType techniqueType)
    {
        switch (techniqueType)
        {
            case TechniqueType::Simple:
                return std::make_unique<SimpleTechnique>(m_globalData);
            case TechniqueType::None:
                return std::make_unique<SimpleTechnique>(m_globalData);
        }

        return std::make_unique<SimpleTechnique>(m_globalData);
    }

    void TechniqueManager::Clear()
    {
        m_techniques.resize(0);
    }
}