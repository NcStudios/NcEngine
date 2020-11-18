#include "MouseFollower.h"
#include "input/Input.h"
#include "Window.h"
#include "Engine.h"
#include "Ecs.h"

using namespace nc;

namespace project
{
    void MouseFollower::FrameUpdate(float dt)
    {
        if(!m_transform)
            m_transform = Ecs::GetComponent<nc::Transform>(m_parentHandle);

        auto screenDim = Window::GetDimensions();
        auto worldX = input::MouseX - screenDim.X() / 2;
        auto worldY = input::MouseY + screenDim.Y() / 2;

        const auto& config = engine::Engine::GetConfig();
        auto z = 5.0f;
        
        auto viewPortDist = config.graphics.farClip - config.graphics.nearClip;
        auto zRatio = (float)viewPortDist / z;

        m_transform->SetPosition({worldX / zRatio, worldY / zRatio, z});
    }
}