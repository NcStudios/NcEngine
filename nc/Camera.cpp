#include "Camera.h"

namespace nc
{
    Camera::Camera(ComponentHandle handle, EntityView parentView)
        : Component(handle, parentView)
    {}
}