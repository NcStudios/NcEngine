#pragma once

#include "Environment.h"
#include "IGraphics.h"
#include "camera/MainCamera.h"
#include "ecs/PointLightSystem.h"
#include "ecs/ParticleEmitterSystem.h"
#include "graphics/NcGraphics.h"
#include "ui/UISystemImpl.h"

#include <memory>

namespace nc
{
namespace asset
{
class NcAsset;
} // namespace asset

namespace config
{
struct GraphicsSettings;
struct ProjectSettings;
} // namespace config

namespace window
{
class WindowImpl;
} // namespace window

namespace graphics
{
// TODO #340: Window should be moved inside graphics instead of being passed here
auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                         const config::GraphicsSettings& graphicsSettings,
                         asset::NcAsset* assetModule,
                         Registry* registry,
                         window::WindowImpl* window) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        NcGraphicsImpl(Registry* registry, std::unique_ptr<IGraphics> graphics, window::WindowImpl* window);

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
        std::unique_ptr<IGraphics> m_graphics;
        ui::UISystemImpl m_ui;
        Environment m_environment;
        ecs::PointLightSystem m_pointLightSystem;
        ecs::ParticleEmitterSystem m_particleEmitterSystem;
    };
} // namespace graphics
} // namespace nc
