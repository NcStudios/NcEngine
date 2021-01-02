#pragma once

#include "graphics/techniques/TechniqueType.h"
#include "MaterialProperties.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    class FrameManager;
    class Model;
    class Technique;

    class Material
    {
        public:
            Material(Technique* technique);

            template<TechniqueType T, class ...Args>
            static Material CreateMaterial(Args&& ... args);

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement();
            #endif
            
            void Submit(class FrameManager& frame, const Model& model) const noexcept;
            
        private:
            Technique* m_technique;
    };

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::PhongShadingTechnique>(const std::vector<std::string>& texturePaths, MaterialProperties& materialProperties);

    template<>
    Material Material::CreateMaterial<nc::graphics::TechniqueType::WireframeTechnique>();
}