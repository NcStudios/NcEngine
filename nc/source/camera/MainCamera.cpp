#include "MainCamera.h"
#include "component/Camera.h"
#include "Ecs.h"

namespace nc::camera
{
    MainCamera* MainCamera::m_instance = nullptr;

    MainCamera::MainCamera()
        : m_transform{ nullptr }
    {
        MainCamera::m_instance = this;
    }

    MainCamera::~MainCamera()
    {
        MainCamera::m_instance = nullptr;
    }

    Transform* MainCamera::GetTransform()
    {
        return MainCamera::m_instance->m_transform;
    }

    void MainCamera::Register(Camera* camera)
    {
        MainCamera::m_instance->m_transform = Ecs::GetComponent<Transform>(camera->GetParentHandle());
    }

    Transform* MainCamera::GetTransform_()
    {
        return m_transform;
    }

    void MainCamera::ClearTransform()
    {
        m_transform = nullptr;
    }
}
