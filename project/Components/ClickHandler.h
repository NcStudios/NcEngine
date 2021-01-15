#pragma once
#include "nc/source/component/Component.h"

namespace project
{
    class ClickHandler : public nc::Component
    {
        public:
            ClickHandler(nc::EntityHandle handle);
            void FrameUpdate(float dt) override;
    };
}