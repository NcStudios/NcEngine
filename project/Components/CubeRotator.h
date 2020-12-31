#pragma once
#include "nc/source/component/Transform.h"

namespace project
{
    class CubeRotator : public nc::Component
    {
        public:
            CubeRotator(nc::ComponentHandle handle, nc::EntityHandle parentHandle);
            void FrameUpdate(float dt) override;
        
        private:
            nc::Transform* m_transform;
    };
}