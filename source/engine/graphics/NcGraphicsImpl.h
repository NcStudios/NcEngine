#pragma once

#include "camera/MainCamera.h"
#include "ecs/PointLightSystem.h"
#include "ecs/ParticleEmitterSystem.h"
#include "Graphics.h"
#include "graphics/NcGraphics.h"
#include "shaders/Environment.h"
#include "task/Job.h"
#include "ui/UISystemImpl.h"

#include <memory>

namespace nc { struct GpuAccessorSignals; }
namespace nc::window { class WindowImpl; }

namespace nc::graphics
{
enum class GraphicsApi
{
    Vulkan_1_0 = 10,
    Vulkan_1_1 = 11,
    Vulkan_1_2 = 12
};

struct GraphicsInitInfo
{
    GpuAccessorSignals gpuAccessorSignals;
    std::string appName;
    uint32_t appVersion;
    GraphicsApi api;
    bool useValidationLayers;
};

// TODO #340: Window should be moved inside graphics instead of being passed here
auto BuildGraphicsModule(bool enableModule, Registry* registry, GraphicsInitInfo info, window::WindowImpl* window) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        // TODO #341: Graphics api should be injected as unqiue_ptr<IGraphics> to remove Vulkan dependency
        NcGraphicsImpl(Registry* registry, GraphicsInitInfo info, window::WindowImpl* window);

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        auto BuildWorkload()->std::vector<task::Job> override;
        void Clear() noexcept override;
        void Run();

        void OnResize(float width, float height, float nearZ, float farZ, WPARAM windowArg);

    private:
        Registry* m_registry;
        camera::MainCamera m_camera;
        Graphics m_graphics;
        ui::UISystemImpl m_ui;
        Environment m_environment;
        ecs::PointLightSystem m_pointLightSystem;
        ecs::ParticleEmitterSystem m_particleEmitterSystem;
    };
} // namespace nc::graphics
