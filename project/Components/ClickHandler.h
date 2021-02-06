#pragma once
#include "component/Component.h"
#include "Input.h"
#include "Physics.h"

namespace project
{
    class ClickHandler : public nc::Component
    {
        public:
            ClickHandler(nc::EntityHandle handle)
                : Component{handle}
            {
            }

            void FrameUpdate(float) override
            {
                if(GetKeyDown(nc::input::KeyCode::LeftButton))
                {
                    auto hit = nc::physics::RaycastToClickables(nc::physics::Layer::GamePiece);
                    if(hit)
                        hit->OnClick();
                }
            }
    };
}