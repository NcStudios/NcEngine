#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/TechniqueType.h"
#include "graphics/techniques/TechniqueManager.h"

#include <string>

namespace nc::graphics
{
    class Material
    {
        public: 
            Material() = default;

            template<class ... Args>
            explicit Material(TechniqueType type, Args&& ... args);
            void SetTechnique(const Technique* technique) noexcept;
            void Submit(class FrameManager& frame, const Model& model) noexcept;
            
        private:
            Technique* m_technique;

    };

    template<class ... Args>
    Material::Material(TechniqueType type, Args&& ... args)
    {
        m_technique = TechniqueManager::GetTechnique(type, std::forward<Args>(args)...);
    }

}