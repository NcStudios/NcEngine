#include "CamController.h"
#include "MainCamera.h"
#include "Input.h"
#include "UI.h"
#include "Window.h"

using namespace nc;

namespace
{
    const float CAM_ZOOM_SPEED = 0.2f;
    const float CAM_PAN_SPEED = 4.0f;
    const unsigned EDGE_PAN_WIDTH = 50;
    const unsigned HUD_HEIGHT = 200;

    const float CamRotateSpeed = 0.1f;
}

CamController::CamController(EntityHandle handle)
    : Component(handle),
      m_mainCameraTransform{nullptr}
{}

void CamController::FrameUpdate(float dt)
{
    if (!m_mainCameraTransform)
    {
        m_mainCameraTransform = camera::GetMainCameraTransform();
    }

    if (ui::IsHovered())
    {
        return;
    }

    Vector3 camTransl = dt * (GetCameraZoomMovement() + GetCameraPanMovement());
    m_mainCameraTransform->Translate(camTransl, Space::Local);

    GetCameraRotation();
}

void CamController::GetCameraRotation()
{
    if(input::GetKey(input::KeyCode::X))
    {
        m_mainCameraTransform->Rotate(Vector3::Right(), CamRotateSpeed);
    }

    if(input::GetKey(input::KeyCode::Y))
    {
        m_mainCameraTransform->Rotate(Vector3::Up(), CamRotateSpeed);
    }
}

Vector3 CamController::GetCameraZoomMovement()
{
    return Vector3{ 0.0f, -1.0f * CAM_ZOOM_SPEED * (float)input::MouseWheel(), 0.0f };
}

Vector3 CamController::GetCameraPanMovement()
{
    auto xPan = 0.0f;
    auto zPan = 0.0f;
    auto [screenWidth, screenHeight] = nc::window::GetDimensions();

    auto [x, y] = input::MousePos();

    if(x < EDGE_PAN_WIDTH)
        xPan = -1.0;
    else if(x > screenWidth - EDGE_PAN_WIDTH)
        xPan = 1.0;

    if(y < EDGE_PAN_WIDTH)
        zPan = 1.0;
    else if(y > screenHeight - EDGE_PAN_WIDTH - HUD_HEIGHT)
        zPan = -1.0;

    return CAM_PAN_SPEED * Normalize(Vector3{xPan, 0.0f, zPan});
}