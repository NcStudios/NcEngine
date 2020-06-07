#include "Head.h"
#include "component/Camera.h"


//temp
#include <iostream>

Head::Head()
{}

void Head::OnInitialize()
{}

void Head::FrameUpdate(float dt)
{
    using namespace nc::input;
    float rotateSpeed = 0.2f * dt;
    float objRotX = 0.0f;
    float objRotY = 0.0f;
    float objRotZ = 0.0f;

    if(GetKey(KeyCode::Y))
        objRotY += 10.0f;
    if(GetKey(KeyCode::X))
        objRotX += 10.0f;
    if(GetKey(KeyCode::Z))
        objRotZ += 10.0f;

    NCE::GetTransform(*GetParentView())->Rotate(objRotX, objRotY, objRotZ, rotateSpeed);
}


void Head::OnDestroy()
{
}

void Head::OnCollisionEnter(const EntityHandle other)
{
    (void)other;
}

