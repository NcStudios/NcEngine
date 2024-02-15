#pragma once

#include "IGraphics.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"
#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/PointLightSystem.h"
#include "system/SkeletalAnimationSystem.h"
#include "system/TextureSystem.h"
#include "system/UISystem.h"
#include "system/WidgetSystem.h"

#include <memory>

namespace nc
{
class Scene;

namespace config
{
struct GraphicsSettings;
struct MemorySettings;
struct ProjectSettings;
} // namespace config

namespace window
{
class WindowImpl;
} // namespace window

namespace graphics
{
struct ShaderResourceBus;

// TODO #340: Window should be moved inside graphics instead of being passed here
auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                         const config::GraphicsSettings& graphicsSettings,
                         const config::MemorySettings& memorySettings,
                         ModuleProvider modules,
                         Registry* registry,
                         window::WindowImpl* window) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       Registry* registry,
                       ModuleProvider modules,
                       std::unique_ptr<IGraphics> graphics,
                       ShaderResourceBus shaderResourceBus,
                       window::WindowImpl* window);

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        void OnBuildTaskGraph(task::TaskGraph&) override;
        void Clear() noexcept override;
        void Run();

        void OnResize(float width, float height, bool isMinimized);

    private:
        Registry* m_registry;
        std::unique_ptr<IGraphics> m_graphics;
        ShaderResourceBus m_shaderResourceBus;
        CameraSystem m_cameraSystem;
        EnvironmentSystem m_environmentSystem;
        ObjectSystem m_objectSystem;
        PointLightSystem m_pointLightSystem;
        ParticleEmitterSystem m_particleEmitterSystem;
        SkeletalAnimationSystem m_skeletalAnimationSystem;
        TextureSystem m_textureSystem;
        WidgetSystem m_widgetSystem;
        UISystem m_uiSystem;
    };
} // namespace graphics
} // namespace nc
