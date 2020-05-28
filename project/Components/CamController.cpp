#include "CamController.h"
#include "Camera.h"



#include <iostream>

CamController::CamController()
{}

void CamController::OnInitialize()
{}

void CamController::FrameUpdate(float dt)
{
    const float zoomSpeed =  2.5f;
    const float panSpeed  =  1.0f;
    const float rotSpeed  = -2.0f;
    Vector3     camTransl = dt * (GetCameraZoomMovement() * zoomSpeed + GetCameraPanMovement() * panSpeed);
    Vector3     camRot    = dt * (GetCameraRotationMovement() * rotSpeed);
    Transform*  transform = View<Camera, Transform>(this);
    transform->CamTranslate(camTransl, 1.0f);
    transform->Rotate(camRot.Y(), camRot.X(), 0.0f, 1.0f);
}

Vector3 CamController::GetCameraZoomMovement()
{
    float wheelInput = input::MouseWheel();
    return Vector3(0.0f, 0.0f, wheelInput);
}

Vector3 CamController::GetCameraPanMovement()
{
    Vector3 targetPosition = Vector3::Zero();

    if(input::GetKey(input::KeyCode::MiddleButton))
    {
        if(!m_camPanState.isActive) //button pressed
        {
            m_camPanState.isActive     = true;
            m_camPanState.initialMouse = Vector2(input::MouseX, input::MouseY);
            m_camPanState.initialCam   = View<Camera, Transform>(this)->GetPosition();
        }
        else //button held
        {
            //const float SCALE      = 0.01f;
            Vector2 curMousePos    = Vector2(input::MouseX, input::MouseY);
            Vector2 mousePosDelta  = (curMousePos - m_camPanState.initialMouse);           
            Vector3 camTranslation = Vector3(mousePosDelta, 0.0f);
            Vector3 curCamPos      = View<Camera, Transform>(this)->GetPosition();
            targetPosition         = m_camPanState.initialCam + camTranslation;
            targetPosition         = (targetPosition - curCamPos);
            //float   distance       = Vector3::SquareMagnitude()
            //float mag              = Vector3::SquareMagnitude(targetPosition);
            //targetPosition         = targetPosition / mag;
            targetPosition.InvertY();
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

    //targetPosition = targetPosition * 0.01f;

    return targetPosition.GetNormalized();
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
            m_camRotateState.initialCam   = View<Camera, Transform>(this)->GetRotation();
        }
        else //button held
        {
            Vector2 currentMousePos  = Vector2(input::MouseX, input::MouseY);
            Vector2 mouseDelta       = m_camRotateState.initialMouse - currentMousePos;
            Vector3 cameraRotation   = Vector3(mouseDelta.X(), mouseDelta.Y(), 0.0f);
            Vector3 currentCameraRot = View<Camera, Transform>(this)->GetRotation();
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

    return targetRotation.GetNormalized();
}