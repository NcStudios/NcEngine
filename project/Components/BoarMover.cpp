#include "BoarMover.h"
#include "input/Input.h"
#include "Ecs.h"

using namespace nc;

namespace
{
    float FACTOR = 10.0f;
}

namespace project
{
    BoarMover::BoarMover(ComponentHandle handle, EntityHandle parentHandle)
        : Component(handle, parentHandle),
          m_transform{Ecs::GetComponent<Transform>(parentHandle)}
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
}