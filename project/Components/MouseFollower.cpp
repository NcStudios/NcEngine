#include "MouseFollower.h"
#include "input/Input.h"
#include "Window.h"
#include "Engine.h"
#include "Ecs.h"

using namespace nc;

namespace project
{
    MouseFollower::MouseFollower(ComponentHandle handle, EntityHandle parentHandle)
    : Component(handle, parentHandle),
      m_screenDimensions { Window::GetDimensions() },
      m_transform { Ecs::GetComponent<nc::Transform>(m_parentHandle) },
      m_zDepth { 0.0f },
      m_zRatio { 0.0f }
    {
        const auto& config = engine::Engine::GetConfig();
        m_viewPortDist = config.graphics.farClip - config.graphics.nearClip;
        Window::RegisterOnResizeReceiver(this);
    }

    MouseFollower::~MouseFollower()
    {
        Window::UnregisterOnResizeReceiver(this);
    }

    void MouseFollower::OnResize(nc::Vector2 screenDimensions)
    {
        m_screenDimensions = screenDimensions;
    }

    void MouseFollower::FrameUpdate(float dt)
    {
        m_zDepth += (float)input::MouseWheel() * dt * 2.0f;
        m_zRatio = m_viewPortDist / m_zDepth;
        auto worldX = input::MouseX - m_screenDimensions.x / 2;
        auto worldY = input::MouseY + m_screenDimensions.y / 2;
        m_transform->SetPosition({worldX / m_zRatio, worldY / m_zRatio, m_zDepth});
    }
}