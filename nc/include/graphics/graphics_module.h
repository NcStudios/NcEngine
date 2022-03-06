#pragma once

#include "ecs/component/Camera.h"
#include "engine_module.h"
#include "ui/IUI.h"

namespace nc
{
    class Registry;

    struct graphics_module : public engine_module
    {
        virtual void set_camera(Camera* camera) noexcept = 0;
        virtual auto get_camera() noexcept -> Camera* = 0;

        virtual void set_ui(ui::IUI* ui) noexcept = 0;
        virtual bool is_ui_hovered() const noexcept = 0;

        virtual void set_skybox(const std::string& path) = 0;
        virtual void clear_environment() = 0;

        virtual void run(Registry* reg) = 0;
    };
}