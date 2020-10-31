#include "CamController.h"
#include "NcCamera.h"
#include "input/Input.h"

//for testing
#include "NcLog.h"

using namespace nc;

namespace
{
    const float CAM_ZOOM_SPEED = 2.5f;
    const float CAM_PAN_SPEED = 70.0f;
    const unsigned EDGE_PAN_WIDTH = 180;
    const unsigned HUD_HEIGHT = 200;
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

    std::string posString = std::to_string(input::MouseX);
    log::NcLogToGame(posString);

    Vector3 camTransl = dt * (GetCameraZoomMovement() + GetCameraPanMovement());
    m_mainCameraTransform->Translate(camTransl, Space::World);
}

Vector3 CamController::GetCameraZoomMovement()
{
    return Vector3{ 0.0f, -1.0f * CAM_ZOOM_SPEED * (float)input::MouseWheel(), 0.0f };
}

Vector3 CamController::GetCameraPanMovement()
{
    auto xPan = 0.0f;
    auto zPan = 0.0f;

    if(input::MouseY > m_config.graphics.screenHeight - HUD_HEIGHT)
    {
        return Vector3::Zero();
    }

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
        zPan = 1.0;
    }
    else if(input::MouseY > m_config.graphics.screenHeight - EDGE_PAN_WIDTH - HUD_HEIGHT)
    {
        zPan = -1.0;
    }

    return CAM_PAN_SPEED * Vector3{ xPan, 0.0f, zPan}.GetNormalized();
}