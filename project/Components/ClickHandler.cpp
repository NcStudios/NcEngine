#include "ClickHandler.h"
#include "input/Input.h"
#include "NcPhysics.h"

namespace project
{
    void ClickHandler::FrameUpdate(float dt)
    {
        (void)dt;

        if(nc::input::GetKeyDown(nc::input::KeyCode::LeftButton))
        {
            nc::physics::NcRaycastToIClickables(nc::physics::Layer::GamePiece);
        }
    }
}