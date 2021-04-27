#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/MaterialProperties.h"
#include "graphics/Mesh.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    namespace d3dresource
    {
        template<typename T>
        class PixelConstantBuffer;
    }

    class ParticleTechnique : public Technique
    {
        public:
            ParticleTechnique();
            
            static size_t GetUID() noexcept;
            static void InitializeCommonResources();
            static void BindCommonResources();

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
            static std::vector<d3dresource::GraphicsResource*> m_commonResources;
            //MaterialProperties m_materialProperties;
            static std::unique_ptr<graphics::Mesh> m_mesh;
            static std::unique_ptr<d3dresource::PixelConstantBuffer<MaterialProperties>> m_materialPropertiesBuffer;
    };
}