#pragma once

#include "utility/SystemBase.h"
#include "component/Camera.h"

namespace nc
{
    class MainCamera : SystemBase
    {
        public:
            virtual void Set(Camera* camera) = 0;
            virtual auto Get() -> Camera* = 0;
    };
}