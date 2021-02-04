#include "CubeRotator.h"
#include "Ecs.h"

using namespace nc;

namespace project
{
    CubeRotator::CubeRotator(EntityHandle handle)
        : Component(handle),
          m_transform{GetComponent<Transform>(handle)}
    {
    }

    void CubeRotator::FrameUpdate(float dt)
    {
        m_transform->Rotate(Vector3::Up(), 0.1f * dt);
    }
}