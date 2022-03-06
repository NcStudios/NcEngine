#pragma once

#include "camera/MainCamera.h"
#include "ecs/PointLightSystem.h"
#include "Graphics.h"
#include "graphics/graphics_module.h"
#include "resources/Environment.h"
#include "ui/UISystemImpl.h"

#include <memory>

/** @todo remove */
#include "task/TaskGraph.h"

namespace nc::window { class WindowImpl; }

namespace nc::graphics
{
    auto build_graphics_module(bool enableModule, Registry* reg, window::WindowImpl* window) -> std::unique_ptr<graphics_module>;

    class graphics_module_impl : public graphics_module
    {
        public:
            graphics_module_impl(Registry* reg, window::WindowImpl* window);

            void set_camera(Camera* camera) noexcept override;
            auto get_camera() noexcept -> Camera* override;

            void set_ui(ui::IUI* ui) noexcept override;
            bool is_ui_hovered() const noexcept override;

            void set_skybox(const std::string& path) override;
            void clear_environment() override;

            void clear() noexcept override;

            void run(Registry* reg) override;

            /** @todo fix */
            auto get_tasks() -> TaskGraph& override { static TaskGraph tg; return tg; }

        private:
            camera::MainCamera m_camera;
            Graphics m_graphics;
            ui::UISystemImpl m_ui;
            Environment m_environment;
            ecs::PointLightSystem m_pointLightSystem;
    };
}