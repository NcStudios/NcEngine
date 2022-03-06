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

    class environment_module : public SystemBase
    {
        public:
            virtual void set_skybox(const std::string& path) = 0;
            virtual void clear_skybox() = 0;

            virtual void set_camera(Camera* camera) noexcept = 0;
            virtual auto get_camera() noexcept -> Camera* = 0;
    };
}