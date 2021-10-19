#pragma once

#include <string>

namespace nc
{
    struct Material
    {
        std::string baseColor; // Slot 0
        std::string normal; // Slot 1
        std::string roughness; // Slot 2

        #ifdef NC_EDITOR_ENABLED
        void EditorGuiElement();
        #endif
    };
}