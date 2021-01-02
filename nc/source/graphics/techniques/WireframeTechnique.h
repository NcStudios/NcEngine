#pragma once

#include "graphics/techniques/Technique.h"
#include "graphics/techniques/TechniqueType.h"

#include <string>
#include <vector>

namespace nc::graphics
{
    class WireframeTechnique : public Technique
    {
        public:
            WireframeTechnique();
            static size_t GetUID() noexcept;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
            
        private:
    };
}