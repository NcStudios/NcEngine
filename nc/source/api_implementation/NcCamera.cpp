#include "NcCamera.h"
#include "camera/MainCamera.h"
#include "NcDebug.h"

namespace nc::camera
{
    void NcRegisterMainCamera(Camera * camera)
    {
        V_LOG("Registering main camera");
        MainCamera::Register(camera);
    }

    Transform * NcGetMainCameraTransform()
    {
        return MainCamera::GetTransform();
    }
}