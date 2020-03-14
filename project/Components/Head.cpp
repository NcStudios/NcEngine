#include "Head.h"

#include <iostream>
#include "Camera.h"

Head::Head(ComponentHandle handle, EntityView parentView) 
    : Component(handle, parentView) {}

void Head::OnInitialize()
{
    rng = std::mt19937(std::random_device{}());
    angleDist = std::uniform_real_distribution(0.0f,3.1415f * 2.0f);
    posDist = std::uniform_real_distribution(-10.0f,10.0f);
    scaleDist = std::uniform_real_distribution(0.5f, 2.0f); 
    colorDist = std::uniform_real_distribution(0.0f, 1.0f);
}

void Head::FrameUpdate()
{
    using namespace nc::input;

    float dt = time::Time::FrameDeltaTime;
    float moveSpeed = 1.0f;
    float rotateSpeed = 0.2f * dt;

    auto camT = View<Camera, Transform>(this);

    Vector3 camMovement = Vector3(GetXAxis(), 0.0f, -1.0f * GetYAxis()).GetNormalized() * dt; 
    camT->CamTranslate(camMovement, moveSpeed);

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

    camT->Rotate(cameraRotationDX, 0.0f, 0.0f, rotateSpeed);
    camT->RotateClamped(0.0f, cameraRotationDY, 0.0f, rotateSpeed, clampMin, clampMax);

    float objRotX = 0.0f;
    float objRotY = 0.0f;
    float objRotZ = 0.0f;

    if(GetKey(KeyCode::Y))
        objRotY += 10.0f;
    if(GetKey(KeyCode::X))
        objRotX += 10.0f;
    if(GetKey(KeyCode::Z))
        objRotZ += 10.0f;

    View<Transform>(this)->Rotate(objRotX, objRotY, objRotZ, rotateSpeed);

    // if(GetKey(KeyCode::Y))
    //     View<Transform>(this)->RotateAround(Vector3::Up(), 0.05f);
    // if(GetKey(KeyCode::X))
    //     View<Transform>(this)->RotateAround(Vector3::Left(), 0.05f);
    // if(GetKey(KeyCode::Z))
    //     View<Transform>(this)->RotateAround(Vector3::Front(), 0.05f);

}


void Head::OnDestroy()
{
}

void Head::OnCollisionEnter(const EntityHandle other)
{

}

