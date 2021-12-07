#pragma once

#include "utility/SystemBase.h"
#include "ecs/component/Camera.h"

namespace nc
{
    class MainCamera : SystemBase
    {
        public:
            virtual void Set(Camera* camera) noexcept = 0;
            virtual auto Get() noexcept -> Camera* = 0;
    };
}