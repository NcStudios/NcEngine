#pragma once
#include "component/Transform.h"

namespace project
{
    class CubeRotator : public nc::Component
    {
        public:
            void FrameUpdate(float dt) override;
        
        private:
            nc::Transform* m_transform;
    };
}