#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/TechniqueType.h"
#include "graphics/materials/MaterialProperties.h"

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
            static size_t GetUID(const std::vector<std::string>& texturePaths, MaterialProperties& materialProperties) noexcept;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
            MaterialProperties m_materialProperties;
            std::unique_ptr<d3dresource::PixelConstantBuffer<MaterialProperties>> m_materialPropertiesBuffer;
    };
}