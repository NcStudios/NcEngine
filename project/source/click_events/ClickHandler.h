#pragma once

#include "component/Component.h"
#include "Input.h"
#include "Physics.h"

namespace nc::sample
{
    /** Raycast to clickables on left mouse click using a given layer mask. */
    class ClickHandler : public AutoComponent
    {
        public:
            ClickHandler(Entity entity, physics::LayerMask mask);
            void FrameUpdate(float) override;
            void SetLayer(physics::LayerMask mask);

        private:
            physics::LayerMask m_mask;
    };

    inline ClickHandler::ClickHandler(Entity entity, physics::LayerMask mask)
        : AutoComponent{entity},
          m_mask{mask}
    {
    }

    inline void ClickHandler::FrameUpdate(float)
    {
        if(GetKeyDown(input::KeyCode::LeftButton))
        {
            auto hit = physics::RaycastToClickables(m_mask);
            if(hit)
                hit->OnClick();
        }
    }

    inline void ClickHandler::SetLayer(physics::LayerMask mask)
    {
        m_mask = mask;
    }
}