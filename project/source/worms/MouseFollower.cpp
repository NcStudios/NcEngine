#include "MouseFollower.h"
#include "Input.h"
#include "config/Config.h"
#include "Ecs.h"

namespace nc::sample
{
    MouseFollower::MouseFollower(EntityHandle handle)
    : Component(handle),
      m_screenDimensions { window::GetDimensions() },
      m_transform { GetComponent<nc::Transform>(handle) },
      m_zDepth { 0.0f },
      m_zRatio { 0.0f }
    {
        const auto& config = config::Get();
        m_viewPortDist = config.graphics.farClip - config.graphics.nearClip;
        window::RegisterOnResizeReceiver(this);
    }

    MouseFollower::~MouseFollower() noexcept
    {
        window::UnregisterOnResizeReceiver(this);
    }

    void MouseFollower::OnResize(nc::Vector2 screenDimensions)
    {
        m_screenDimensions = screenDimensions;
    }

    void MouseFollower::FrameUpdate(float dt)
    {
        m_zDepth += (float)input::MouseWheel() * dt * 2.0f;
        m_zRatio = m_viewPortDist / m_zDepth;
        auto worldX = input::MouseX() + m_screenDimensions.x / 2;
        auto worldY = input::MouseY() + m_screenDimensions.y / 2;
        m_transform->SetPosition(Vector3{worldX / m_zRatio, worldY / m_zRatio, m_zDepth});
    }
}