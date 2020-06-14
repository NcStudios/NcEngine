#include "CamController.h"
#include "component/Camera.h"

namespace
{
    const float CAM_ZOOM_SPEED    =  2.5f;
    const float CAM_PAN_SPEED     =  25.0f;
    const float CAM_ROTATE_SPEED  = -1.2f;
}

CamController::CamController()
{}

void CamController::OnInitialize()
{
    m_mainCameraTransform = NCE::GetMainCameraTransform();
}

void CamController::FrameUpdate(float dt)
{
    Vector3 camTransl = dt * (GetCameraZoomMovement() + GetCameraPanMovement());
    Vector3 camRot = dt * (GetCameraRotationMovement());
    m_mainCameraTransform->CamTranslate(camTransl, 1.0f);
    m_mainCameraTransform->Rotate(camRot.Y(), camRot.X(), 0.0f, 1.0f);
}

Vector3 CamController::GetCameraZoomMovement()
{
    return Vector3{ 0.0f, 0.0f, CAM_ZOOM_SPEED * (float)input::MouseWheel() };
}

Vector3 CamController::GetCameraPanMovement()
{
    Vector3 targetPosition = {};

    if(input::GetKey(input::KeyCode::MiddleButton))
    {
        if(!m_camPanState.isActive) //button pressed
        {
            m_camPanState.isActive     = true;
            m_camPanState.initialMouse = Vector2{ (float)input::MouseX, (float)input::MouseY };
            m_camPanState.initialCam   = NCE::GetTransformFromEntityHandle(GetParentHandle())->GetPosition();
        }
        else //button held
        {
            auto curMousePos = Vector2{ (float)input::MouseX, (float)input::MouseY };
            auto mousePosDelta = curMousePos - m_camPanState.initialMouse;
            auto camTranslation = Vector3{ mousePosDelta, 0.0f };
            //auto curCamPos = m_mainCameraTransform->GetPosition();
            auto curCamPos = NCE::GetTransformFromEntityHandle(GetParentHandle())->GetPosition();
            targetPosition = m_camPanState.initialCam + camTranslation;
            targetPosition = targetPosition - curCamPos;
            targetPosition.InvertX();
        }
    }
    else
    {
        if(m_camPanState.isActive) //button released
        {
            m_camPanState.isActive     = false;
            m_camPanState.initialMouse = Vector2::Zero();
            m_camPanState.initialCam   = Vector3::Zero();
        }
    }
    
    return CAM_PAN_SPEED * targetPosition.GetNormalized();
}

Vector3 CamController::GetCameraRotationMovement()
{
    Vector3 targetRotation = Vector3::Zero();

    if(input::GetKey(input::KeyCode::RightButton))
    {
        if(!m_camRotateState.isActive) //button pressed
        {
            m_camRotateState.isActive     = true;
            m_camRotateState.initialMouse = Vector2(input::MouseX, input::MouseY);
            m_camRotateState.initialCam   = NCE::GetTransformFromHandle(GetParentHandle())->GetRotation();
        }
        else //button held
        {
            Vector2 currentMousePos  = Vector2(input::MouseX, input::MouseY);
            Vector2 mouseDelta       = m_camRotateState.initialMouse - currentMousePos;
            Vector3 cameraRotation   = Vector3(mouseDelta.X(), mouseDelta.Y(), 0.0f);
            Vector3 currentCameraRot = NCE::GetTransformFromEntityHandle(GetParentHandle())->GetRotation();
            targetRotation           = m_camRotateState.initialCam + cameraRotation;
            targetRotation           = (targetRotation - currentCameraRot);
        }
    }
    else
    {
        if(m_camRotateState.isActive) //button released
        {
            m_camRotateState.isActive     = false;
            m_camRotateState.initialMouse = Vector2::Zero();
            m_camRotateState.initialCam   = Vector3::Zero();
        }
    }

    return CAM_ROTATE_SPEED * targetRotation.GetNormalized();
}