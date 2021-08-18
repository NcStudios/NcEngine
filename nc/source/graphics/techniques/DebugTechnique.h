#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/MaterialProperties.h"
#include "graphics/Mesh.h"

#include <vector>

namespace nc::graphics
{
    namespace d3dresource
    {
        template<typename T>
        class PixelConstantBuffer;
    }

    class DebugTechnique : public Technique
    {
        public:
            DebugTechnique();
            static void InitializeCommonResources();
            static void BindCommonResources();

            static size_t GetUID() noexcept { return 987; }

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override {}
            #endif

        private:
            static std::vector<d3dresource::GraphicsResource*> m_commonResources;
            static std::unique_ptr<graphics::Mesh> m_lineMesh;
            static std::unique_ptr<graphics::Mesh> m_pointMesh;
    };
}