#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/TechniqueType.h"
#include "graphics/materials/MaterialProperties.h"
#include "graphics/d3dresource/ConstantBufferResources.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    namespace d3dresource
    {
        template<typename T>
        class PixelConstantBuffer;
    }

    class PhongShadingTechnique : public Technique
    {
        public:
            PhongShadingTechnique(const std::vector<std::string>& texturePaths, MaterialProperties materialProperties = {});
            static size_t GetUID(TechniqueType type, const std::vector<std::string>& texturePaths) noexcept;

        private:
            std::unique_ptr<d3dresource::PixelConstantBuffer<MaterialProperties>> m_materialPropertiesBuffer;
    };
}