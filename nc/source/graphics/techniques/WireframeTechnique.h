#pragma once

#include "graphics/techniques/Technique.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    class WireframeTechnique : public Technique
    {
        public:
            WireframeTechnique();
            static size_t GetUID() noexcept;
            static void InitializeCommonResources();
            static void BindCommonResources();

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
            static std::vector<d3dresource::GraphicsResource*> m_commonResources;
    };
}