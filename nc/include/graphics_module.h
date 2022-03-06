#pragma once

#include "ecs/component/Camera.h"
#include "engine_module.h"

namespace nc
{
    struct graphics_module : public engine_module
    {
        virtual void set_skybox(const std::string& path) = 0;
        virtual void clear_skybox() = 0;

        virtual void set_camera(Camera* camera) noexcept = 0;
        virtual auto get_camera() noexcept -> Camera* = 0;
    };
}