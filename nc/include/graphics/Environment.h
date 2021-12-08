#pragma once

#include "Assets.h"
#include "utility/SystemBase.h"

namespace nc
{
    class Environment : public SystemBase
    {
        public:
            virtual void SetSkybox(const CubeMapFaces& skybox) = 0;
    };
}