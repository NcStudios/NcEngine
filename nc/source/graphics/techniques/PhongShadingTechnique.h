#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/TechniqueType.h"
#include "graphics/materials/MaterialProperties.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    class PhongShadingTechnique : public Technique
    {
        public:
            PhongShadingTechnique(const std::vector<std::string>& texturePaths, MaterialProperties materialProperties = {});
            static size_t GetUID(TechniqueType type, const std::vector<std::string>& texturePaths) noexcept;

    };
}