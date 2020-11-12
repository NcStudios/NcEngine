#pragma once

#include "component/Component.h"

namespace project
{
    class Sample : public nc::Component
    {
        public:
            void FrameUpdate(float dt) override;

        private:

    };
}