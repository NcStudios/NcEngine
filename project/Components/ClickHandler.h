#pragma once
#include "nc/source/component/Component.h"

namespace project
{
    class ClickHandler : public nc::Component
    {
        public:
            ClickHandler(nc::ComponentHandle handle, nc::EntityHandle parentHandle);
            void FrameUpdate(float dt) override;
    };
}