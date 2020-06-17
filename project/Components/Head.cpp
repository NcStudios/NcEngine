#include "Head.h"
#include "component/Camera.h"


//temp
#include <iostream>

Head::Head()
{}

void Head::FrameUpdate(float dt)
{
    if (!m_transform)
    {
        m_transform = NCE::GetTransformFromEntityHandle(m_parentHandle);
    }

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

    m_transform->Rotate(objRotX, objRotY, objRotZ, rotateSpeed);
}


void Head::OnDestroy()
{
}

void Head::OnCollisionEnter(const EntityHandle other)
{
    (void)other;
}

