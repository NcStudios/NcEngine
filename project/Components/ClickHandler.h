#pragma once
#include "component/Component.h"

namespace project
{
    class ClickHandler : public nc::Component
    {
        public:
            void FrameUpdate(float dt) override;
    };
}