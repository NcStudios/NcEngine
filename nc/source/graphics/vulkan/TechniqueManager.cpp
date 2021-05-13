#include "graphics/vulkan/TechniqueManager.h"
#include "graphics/Graphics2.h"
#include "graphics/vulkan/Commands.h"
#include "graphics/vulkan/techniques/PhongAndUiTechnique.h"
#include "component/vulkan/MeshRenderer.h"
#include "component/Transform.h"

namespace nc::graphics::vulkan
{
    TechniqueManager::TechniqueManager(nc::graphics::Graphics2* graphics)
    : m_graphics{graphics},
      m_techniques{}
    {
    }

    void TechniqueManager::RegisterMeshRenderer(TechniqueType technique, MeshRenderer* meshRenderer)
    {
        for (auto& registeredTechnique : m_techniques)
        {
            if (registeredTechnique->GetType() == technique)
            {
                registeredTechnique->RegisterMeshRenderer(meshRenderer);
                return;
            }
        }

        auto techniqueToRegister = CreateTechnique(technique);
        techniqueToRegister->RegisterMeshRenderer(meshRenderer);
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
                return std::make_unique<PhongAndUiTechnique>(m_graphics);
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