#include "Material.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Model.h"
#include "graphics/techniques/TechniqueManager.h"
#include "Engine.h"
#include "config/Config.h"

namespace nc::graphics
{
    Material::Material(const std::vector<std::string>& texturePaths, TechniqueType technique)
    {
        m_technique = TechniqueManager::GetTechnique(technique, texturePaths);
    }

    void Material::Submit(FrameManager& frame, const Model& model) noexcept
    {
        m_technique->Submit(frame, model);
    }
}