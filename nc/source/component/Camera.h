#pragma once
#include "Component.h"

namespace nc
{
    class Camera final: public Component
    {
        public:
            Camera();

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif

    };
}