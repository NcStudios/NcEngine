#pragma once

#include "component/Component.h"
#include "component/Transform.h"

namespace project
{
    class BoarMover : public nc::Component
    {
        public:
            BoarMover(nc::EntityHandle parentHandle, bool update);
            void FrameUpdate(float dt) override;
            void OnCollisionEnter(nc::Entity* other) override;
            void OnCollisionExit(nc::Entity* other) override;
            void OnCollisionStay(nc::Entity* other) override;

        private:
            nc::Transform* m_transform;
            bool m_useInput;
    };
}