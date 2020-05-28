#pragma once
#include "Common.h"
#include "Component.h"

namespace nc
{
    class Camera : public Component
    {
        public:
            Camera();

            #ifdef NC_DEBUG
            void EditorGuiElement() override;
            #endif

    };
}