#include "BoarMover.h"
#include "Input.h"
#include "Ecs.h"

#include <iostream>

using namespace nc;

namespace
{
    float FACTOR = 10.0f;
}

namespace project
{
    BoarMover::BoarMover(EntityHandle parentHandle)
        : Component(parentHandle),
          m_transform{GetComponent<Transform>(parentHandle)}
    {}

    void BoarMover::FrameUpdate(float dt)
    {
        float Factor = FACTOR * dt;
        
        if(input::GetKeyDown(input::KeyCode::W))
        {
            m_transform->Translate(Vector3::Front() * Factor);
        }

        if(input::GetKeyDown(input::KeyCode::A))
        {
            m_transform->Translate(Vector3::Left() * Factor);
        }

        if(input::GetKeyDown(input::KeyCode::S))
        {
            m_transform->Translate(Vector3::Back() * Factor);
        }

        if(input::GetKeyDown(input::KeyCode::D))
        {
            m_transform->Translate(Vector3::Right() * Factor);
        }

        if(input::GetKeyDown(input::KeyCode::Q))
        {
            m_transform->Translate(Vector3::Up() * Factor);
        }

        if(input::GetKeyDown(input::KeyCode::E))
        {
            m_transform->Translate(Vector3::Down() * Factor);
        }
    }

    void BoarMover::OnCollisionEnter(Entity*)
    {
        std::cout << "collision enter\n";
    }

    void BoarMover::OnCollisionStay(Entity*)
    {
        std::cout << "collision stay\n";
    }

    void BoarMover::OnCollisionExit(Entity*)
    {
        std::cout << "collision exit\n";
    }
}