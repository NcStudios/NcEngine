#pragma once

#include "component/Component.h"
#include "component/Transform.h"

namespace project
{
    class BoarMover : public nc::Component
    {
        public:
            BoarMover(nc::ComponentHandle handle, nc::EntityHandle parentHandle);
            void FrameUpdate(float dt) override;

        private:
            nc::Transform* m_transform;
    };
}