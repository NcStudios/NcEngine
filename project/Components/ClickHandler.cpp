#include "ClickHandler.h"
#include "input/Input.h"
#include "Physics.h"

namespace project
{
    void ClickHandler::FrameUpdate(float dt)
    {
        (void)dt;

        if(nc::input::GetKeyDown(nc::input::KeyCode::LeftButton))
        {
            auto hit = nc::physics::Physics::RaycastToClickables(nc::physics::Layer::GamePiece);
            if(hit)
            {
                hit->OnClick();
            }
        }
    }
}