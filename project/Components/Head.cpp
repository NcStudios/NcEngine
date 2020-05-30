#include "Head.h"
#include "Camera.h"


//temp
#include <iostream>

Head::Head(const char * testString, const int testInt)
    : m_testString(testString), m_testInt(testInt)
{}

void Head::OnInitialize()
{}

void Head::FrameUpdate(float dt)
{
    std::cout << "s: " << m_testString << '\n'
              << "i: " << m_testInt << '\n';

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

    View<Transform>(this)->Rotate(objRotX, objRotY, objRotZ, rotateSpeed);
}


void Head::OnDestroy()
{
}

void Head::OnCollisionEnter(const EntityHandle other)
{

}

