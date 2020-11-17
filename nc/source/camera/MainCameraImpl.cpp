#include "MainCameraImpl.h"
#include "component/Camera.h"
#include "Ecs.h"

namespace nc::camera
{
    MainCameraImpl::MainCameraImpl()
        : m_transform{ nullptr }
    {
    }

    Transform* MainCameraImpl::GetTransform()
    {
        return m_transform;
    }

    void MainCameraImpl::Set(Camera* camera)
    {
        m_transform = Ecs::GetComponent<Transform>(camera->GetParentHandle());
    }

    void MainCameraImpl::ClearTransform()
    {
        m_transform = nullptr;
    }
}
