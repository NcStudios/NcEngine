#include "ClickHandler.h"
#include "input/Input.h"
#include "Physics.h"

using namespace nc;

namespace project
{
    ClickHandler::ClickHandler(ComponentHandle handle, EntityHandle parentHandle)
        : Component(handle, parentHandle)
    {
    }

    void ClickHandler::FrameUpdate(float dt)
    {
        (void)dt;

        if(nc::input::GetKeyDown(input::KeyCode::LeftButton))
        {
            auto hit = physics::Physics::RaycastToClickables(physics::Layer::GamePiece);
            if(hit)
            {
                hit->OnClick();
            }
        }
    }
}