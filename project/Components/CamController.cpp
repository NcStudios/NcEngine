#include "CamController.h"
#include "NcCamera.h"
#include "input/Input.h"

using namespace nc;

namespace
{
    const float CAM_ZOOM_SPEED = 2.5f;
    const float CAM_PAN_SPEED = 70.0f;
    const unsigned EDGE_PAN_WIDTH = 180;
}

CamController::CamController()
    : m_config{ config::NcGetConfigReference() }
{}

void CamController::FrameUpdate(float dt)
{
    if (!m_mainCameraTransform)
    {
        m_mainCameraTransform = NcGetMainCameraTransform();
    }

    Vector3 camTransl = dt * (GetCameraZoomMovement() + GetCameraPanMovement());
    m_mainCameraTransform->Translate(camTransl, Space::Local);
}

Vector3 CamController::GetCameraZoomMovement()
{
    return Vector3{ 0.0f, 0.0f, CAM_ZOOM_SPEED * (float)input::MouseWheel() };
}

Vector3 CamController::GetCameraPanMovement()
{
    auto xPan = 0.0f;
    auto yPan = 0.0f;

    if(input::MouseX < EDGE_PAN_WIDTH)
    {
        xPan = -1.0;
    }
    else if(input::MouseX > m_config.graphics.screenWidth - EDGE_PAN_WIDTH)
    {
        xPan = 1.0;
    }
    if(input::MouseY < EDGE_PAN_WIDTH)
    {
        yPan = 1.0;
    }
    else if(input::MouseY > m_config.graphics.screenHeight - EDGE_PAN_WIDTH)
    {
        yPan = -1.0;
    }

    return CAM_PAN_SPEED * Vector3{ xPan, yPan, 0}.GetNormalized();
}