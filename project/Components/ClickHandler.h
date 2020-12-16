#pragma once
#include "nc/source/component/Component.h"

namespace project
{
    class ClickHandler : public nc::Component
    {
        public:
            void FrameUpdate(float dt) override;
    };
}