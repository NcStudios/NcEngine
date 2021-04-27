#pragma once

#include "graphics/TechniqueType.h"
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
            
            void Submit(FrameManager* frame, const Model* model) const noexcept;
            
        private:
            Technique* m_technique;
    };

    template<>
    Material Material::CreateMaterial<TechniqueType::PhongShading>(const std::vector<std::string>& texturePaths, const MaterialProperties& materialProperties);

    template<>
    Material Material::CreateMaterial<TechniqueType::Wireframe>();

    template<>
    Material Material::CreateMaterial<TechniqueType::Particle>();
}