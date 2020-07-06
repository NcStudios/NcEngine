#include "Ship.h"

using namespace nc;

Ship::Ship()
    : m_transform{ nullptr },
      m_moveSpeed{ 1.0f }
{
}

void Ship::FrameUpdate(float dt)
{
    (void)dt;

    if (!m_transform)
    {
        m_transform = NCE::GetTransformFromEntityHandle(m_parentHandle);
    }

    using namespace nc::input;

    int forward = 0;
    int strafe = 0;
    int up = 0;

    if(GetKey(KeyCode::W))
        forward += 1;
    if(GetKey(KeyCode::S))
        forward -= 1;

    if(GetKey(KeyCode::D))
        strafe += 1;
    if(GetKey(KeyCode::A))
        strafe -= 1;

    if(GetKey(KeyCode::Q))
        up += 1;
    if(GetKey(KeyCode::E))
        up -= 1;

    m_transform->Translate({strafe * m_moveSpeed, up * m_moveSpeed , forward * m_moveSpeed});

    //m_transform->Rotate(objRotX, objRotY, objRotZ, rotateSpeed);
}


void Ship::OnDestroy()
{
}

void Ship::OnCollisionEnter(const EntityHandle other)
{
    (void)other;
}