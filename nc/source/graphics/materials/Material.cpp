#include "Material.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Model.h"
#include "graphics/techniques/TechniqueManager.h"
#include "Engine.h"
#include "config/Config.h"

namespace nc::graphics
{
    void Material::Submit(FrameManager& frame, const Model& model) const noexcept
    {
        m_technique->Submit(frame, model);
    }

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::PhongShadingTechnique>(const std::vector<std::string>& texturePaths, MaterialProperties& materialProperties)
    {
        auto material = Material();
        material.m_technique = TechniqueManager::GetTechnique<PhongShadingTechnique>(texturePaths, materialProperties);
        return material;
    }

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::WireframeTechnique>()
    {
        auto material = Material();
        material.m_technique = TechniqueManager::GetTechnique<WireframeTechnique>();
        return material;
    }

    #ifdef NC_EDITOR_ENABLED
    void Material::EditorGuiElement()
    {
        m_technique->EditorGuiElement();
    }
    #endif
            
}