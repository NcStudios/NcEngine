#pragma once

#include "ecs/component/Component.h"
#include "Input.h"
#include "physics/PhysicsSystem.h"

namespace nc::sample
{
    /** Raycast to clickables on left mouse click using a given layer mask. */
    class ClickHandler : public AutoComponent
    {
        public:
            ClickHandler(Entity entity, LayerMask mask, PhysicsSystem* physicsSystem);
            void FrameUpdate(float) override;
            void SetLayer(LayerMask mask);

        private:
            LayerMask m_mask;
            PhysicsSystem* m_physicsSystem;
    };

    inline ClickHandler::ClickHandler(Entity entity, LayerMask mask, PhysicsSystem* physicsSystem)
        : AutoComponent{entity},
          m_mask{mask},
          m_physicsSystem{physicsSystem}
    {
    }

    inline void ClickHandler::FrameUpdate(float)
    {
        if(GetKeyDown(input::KeyCode::LeftButton))
        {
            auto hit = m_physicsSystem->RaycastToClickables(m_mask);
            if(hit)
                hit->OnClick();
        }
    }

    inline void ClickHandler::SetLayer(LayerMask mask)
    {
        m_mask = mask;
    }
}