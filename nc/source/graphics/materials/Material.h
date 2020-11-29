#pragma once

#include "graphics/rendergraph/Technique.h"
#include "graphics/techniques/TechniqueType.h"

#include <string>

namespace nc::graphics
{
    class Material
    {
        public: 
            Material() = default;
            Material(const std::vector<std::string>& texturePaths, TechniqueType type = TechniqueType::PhongShadingTechnique);
            void SetTechnique(const Technique* technique) noexcept;
            void Submit(class FrameManager& frame, const Model& model) noexcept;
            
        private:
            Technique* m_technique;

    };
}