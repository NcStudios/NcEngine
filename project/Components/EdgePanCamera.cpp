#include "EdgePanCamera.h"
#include "MainCamera.h"
#include "Input.h"
#include "UI.h"
#include "Window.h"

using namespace nc;

namespace
{
    const float ZoomSpeed = 0.9f;
    const float PanSpeed = 10.0f;
    const float CamRotateSpeed = 0.05f;
    const unsigned EdgePanWidth = 50u;
    const unsigned HudHeight = 200u;

    float GetPan(float val, float min, float max)
    {
        if(val < min) return -1.0f;
        if(val > max) return 1.0f;
        return 0.0f;
    };
}

EdgePanCamera::EdgePanCamera(EntityHandle handle)
    : Component(handle),
      m_mainCameraTransform{nullptr},
      m_lastFrameZoom{0.0f}
{}

void EdgePanCamera::FrameUpdate(float dt)
{
    if (!m_mainCameraTransform)
        m_mainCameraTransform = camera::GetMainCameraTransform();

    if (ui::IsHovered())
        return;

    auto [screenWidth, screenHeight] = nc::window::GetDimensions();
    auto [x, y] = input::MousePos();
    auto xPan = GetPan(x, EdgePanWidth, screenWidth - EdgePanWidth);
    auto yPan = -1.0f * GetPan(y, EdgePanWidth, screenHeight - EdgePanWidth - HudHeight);
    auto pan = Normalize(Vector2{xPan, yPan}) * PanSpeed;
    auto zoom = (float)input::MouseWheel() * ZoomSpeed;
    auto translation = math::Lerp(m_lastFrameTranslation, Vector3{pan.x, pan.y, zoom}, 0.1f);
    if(translation == Vector3::Zero()) // b/c float equality
        translation = Vector3::Zero();
    m_lastFrameTranslation = translation;
    translation = translation * dt;
    m_mainCameraTransform->Translate(translation, Space::Local);

    if(input::GetKey(input::KeyCode::X))
        m_mainCameraTransform->Rotate(Vector3::Right(), CamRotateSpeed);

    if(input::GetKey(input::KeyCode::Y))
        m_mainCameraTransform->Rotate(Vector3::Up(), CamRotateSpeed);
}