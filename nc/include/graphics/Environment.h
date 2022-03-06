#pragma once

#include "Assets.h"
#include "ecs/component/Camera.h"
#include "utility/SystemBase.h"

namespace nc
{
    class Environment : public SystemBase
    {
        public:
            virtual void SetSkybox(const std::string& path) = 0;
            virtual void Clear() = 0;
    };
}