#pragma once
#include "Common.h"
#include "Component.h"

namespace nc
{
    class Camera : public Component
    {
        public:
            Camera(ComponentHandle handle, EntityView parentView);

            void EditorGuiElement() override;

    };
}