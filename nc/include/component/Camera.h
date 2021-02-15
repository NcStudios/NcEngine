#pragma once
#include "Component.h"

namespace nc
{
    class Camera : public Component
    {
        public:
            Camera(EntityHandle handle) noexcept;

            #ifdef NC_EDITOR_ENABLED
            void EditorGuiElement() override;
            #endif
    };
}