#include "CamController.h"
#include "Camera.h"

CamController::CamController(ComponentHandle handle, EntityView parentView) 
    : Component(handle, parentView) {}

void CamController::OnInitialize()
{
    cameraTransform = View<Camera, Transform>(this);
}

void CamController::FrameUpdate(float dt)
{
    using namespace nc::input;

    float moveSpeed = 5.0f;
    float rotateSpeed = 0.2f * dt;

    Vector3 camMovement = Vector3(GetXAxis(), 0.0f, -1.0f * GetYAxis()).GetNormalized() * dt; 
    cameraTransform->CamTranslate(camMovement, moveSpeed);

    float cameraRotationDX = 0.0f;
    float cameraRotationDY = 0.0f;

    if(GetKey(KeyCode::UpArrow))
        cameraRotationDX += 1.0f;
    if(GetKey(KeyCode::DownArrow))
        cameraRotationDX -= 1.0f;
    if(GetKey(KeyCode::RightArrow))
        cameraRotationDY += 1.0f;
    if(GetKey(KeyCode::LeftArrow))
        cameraRotationDY -= 1.0f;

    const float clampMin = -3.14159 / 3.0f;
    const float clampMax =  3.14159 / 3.0f;

    cameraTransform->Rotate(cameraRotationDX, 0.0f, 0.0f, rotateSpeed);
    cameraTransform->RotateClamped(0.0f, cameraRotationDY, 0.0f, rotateSpeed, clampMin, clampMax);
}