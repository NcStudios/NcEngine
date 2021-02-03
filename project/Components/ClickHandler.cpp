#include "ClickHandler.h"
#include "Input.h"
#include "Physics.h"

using namespace nc;

namespace project
{
    ClickHandler::ClickHandler(EntityHandle handle)
        : Component(handle)
    {
    }

    void ClickHandler::FrameUpdate(float dt)
    {
        (void)dt;

        if(nc::input::GetKeyDown(input::KeyCode::LeftButton))
        {
            auto hit = physics::RaycastToClickables(physics::Layer::GamePiece);
            if(hit)
            {
                hit->OnClick();
            }
        }
    }
}