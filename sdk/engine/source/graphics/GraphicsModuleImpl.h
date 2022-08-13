#pragma once

#include "camera/MainCamera.h"
#include "ecs/PointLightSystem.h"
#include "ecs/ParticleEmitterSystem.h"
#include "Graphics.h"
#include "graphics/GraphicsModule.h"
#include "resources/Environment.h"
#include "task/Job.h"
#include "ui/UISystemImpl.h"

#include <memory>

namespace nc { struct GpuAccessorSignals; }
namespace nc::window { class WindowImpl; }

namespace nc::graphics
{
    auto BuildGraphicsModule(bool enableModule, Registry* reg, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window, float* dt)->std::unique_ptr<GraphicsModule>;

    class GraphicsModuleImpl : public GraphicsModule
    {
        public:
            GraphicsModuleImpl(Registry* registry, const nc::GpuAccessorSignals& gpuAccessorSignals, window::WindowImpl* window, float* dt);

            void SetCamera(Camera* camera) noexcept override;
            auto GetCamera() noexcept -> Camera* override;
            void SetUi(ui::IUI* ui) noexcept override;
            bool IsUiHovered() const noexcept override;
            void SetSkybox(const std::string& path) override;
            void ClearEnvironment() override;
            auto BuildWorkload()->std::vector<task::Job> override;
            void Clear() noexcept override;
            void Run();

        private:
            Registry* m_registry;
            camera::MainCamera m_camera;
            Graphics m_graphics;
            ui::UISystemImpl m_ui;
            Environment m_environment;
            ecs::PointLightSystem m_pointLightSystem;
            ecs::ParticleEmitterSystem m_particleEmitterSystem;
        };
}