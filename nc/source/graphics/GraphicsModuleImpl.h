#pragma once

#include "camera/MainCamera.h"
#include "ecs/PointLightSystem.h"
#include "Graphics.h"
#include "graphics/GraphicsModule.h"
#include "resources/Environment.h"
#include "task/Job.h"
#include "ui/UISystemImpl.h"

#include <memory>


namespace nc::window { class WindowImpl; }

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* reg, window::WindowImpl* window) -> std::unique_ptr<GraphicsModule>;

    class GraphicsModuleImpl : public GraphicsModule
    {
        public:
            GraphicsModuleImpl(Registry* registry, window::WindowImpl* window);

            void SetCamera(Camera* camera) noexcept override;
            auto GetCamera() noexcept -> Camera* override;
            void SetUi(ui::IUI* ui) noexcept override;
            bool IsUiHovered() const noexcept override;
            void SetSkybox(const std::string& path) override;
            void ClearEnvironment() override;
            auto BuildWorkload() -> std::vector<Job> override;
            void Clear() noexcept override;
            void Run();

        private:
            Registry* m_registry;
            camera::MainCamera m_camera;
            Graphics m_graphics;
            ui::UISystemImpl m_ui;
            Environment m_environment;
            ecs::PointLightSystem m_pointLightSystem;
    };
}