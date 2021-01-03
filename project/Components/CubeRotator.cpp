#include "CubeRotator.h"
#include "Ecs.h"

using namespace nc;

namespace project
{
    CubeRotator::CubeRotator(ComponentHandle handle, EntityHandle parentHandle)
        : Component(handle, parentHandle),
          m_transform{Ecs::GetComponent<Transform>(m_parentHandle)}
    {
    }

    void CubeRotator::FrameUpdate(float dt)
    {
        m_transform->Rotate(0.1f, 0.1f, 0.1f, dt);
    }
}