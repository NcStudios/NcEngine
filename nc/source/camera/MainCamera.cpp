#include "MainCamera.h"
#include "component/Camera.h"
#include "NcCommon.h"

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
        auto handle = camera->GetParentHandle();
        auto entity = NcGetEntity(handle);
        MainCamera::m_instance->m_transform = NcGetTransform(entity->Handles.transform);
    }

    void MainCamera::ClearTransform()
    {
        m_transform = nullptr;
    }
}
