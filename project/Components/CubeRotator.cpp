#include "CubeRotator.h"
#include "Ecs.h"

namespace project
{
    void CubeRotator::FrameUpdate(float dt)
    {
        if(!m_transform)
        {
            m_transform = nc::Ecs::GetComponent<nc::Transform>(m_parentHandle);
        }

        m_transform->Rotate(0.1f, 0.1f, 0.1f, dt);
    }
}