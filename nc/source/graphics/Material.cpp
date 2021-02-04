#include "graphics/Material.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/d3dresource/ConstantBufferResources.h"
#include "graphics/Model.h"
#include "graphics/techniques/TechniqueManager.h"
#include "graphics/techniques/PhongShadingTechnique.h"
#include "graphics/techniques/WireframeTechnique.h"
#include "config/Config.h"

namespace nc::graphics
{
    Material::Material(Technique* technique)
        : m_technique{technique}
    {}

    void Material::Submit(FrameManager& frame, const Model* model) const noexcept
    {
        m_technique->Submit(frame, model);
    }

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::PhongShading>(const std::vector<std::string>& texturePaths, MaterialProperties& materialProperties)
    {
        return Material{TechniqueManager::GetTechnique<PhongShadingTechnique>(texturePaths, materialProperties)};
    }

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::Wireframe>()
    {
        return Material{TechniqueManager::GetTechnique<WireframeTechnique>()};
    }

    #ifdef NC_EDITOR_ENABLED
    void Material::EditorGuiElement()
    {
        m_technique->EditorGuiElement();
    }
    #endif
}