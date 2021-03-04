#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/MaterialProperties.h"

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
            
            static size_t GetUID(const std::vector<std::string>& texturePaths, const MaterialProperties& materialProperties) noexcept;
            static void InitializeCommonResources();
            static void BindCommonResources();

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
            static std::vector<d3dresource::GraphicsResource*> m_commonResources;
            MaterialProperties m_materialProperties;
            std::unique_ptr<d3dresource::PixelConstantBuffer<MaterialProperties>> m_materialPropertiesBuffer;
    };
}