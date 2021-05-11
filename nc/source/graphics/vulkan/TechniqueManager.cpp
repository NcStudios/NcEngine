#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/techniques/PhongAndUiTechnique.h"
#include "component/Transform.h"

namespace nc::graphics::vulkan
{
    TechniqueManager::TechniqueManager(nc::graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_techniques{},
      m_globalData{}
    {
    }

    void TechniqueManager::RegisterGlobalData(vk::Buffer* vertexBuffer, vk::Buffer* indexBuffer)
    {
        m_globalData.vertexBuffer = vertexBuffer;
        m_globalData.indexBuffer = indexBuffer;
    }

    void TechniqueManager::RegisterMeshRenderer(TechniqueType technique, Mesh mesh,  graphics::vulkan::Texture texture, Transform* transform)
    {
        for (auto& registeredTechnique : m_techniques)
        {
            if (registeredTechnique->GetType() == technique)
            {
                registeredTechnique->RegisterMeshRenderer(std::move(mesh),  std::move(texture), transform);
                return;
            }
        }

        auto techniqueToRegister = CreateTechnique(technique);
        techniqueToRegister->RegisterMeshRenderer(std::move(mesh),  std::move(texture), transform);
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
                return std::make_unique<PhongAndUiTechnique>(&m_globalData, m_graphics);
            case TechniqueType::None:
                throw std::runtime_error("Technique not implemented.");
        }

        throw std::runtime_error("Invalid technique chosen.");
    }

    void TechniqueManager::Clear()
    {
        m_techniques.resize(0);
    }
}