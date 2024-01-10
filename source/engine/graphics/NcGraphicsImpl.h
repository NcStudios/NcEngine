#pragma once

#include "IGraphics.h"
#include "graphics/NcGraphics.h"
#include "system/CameraSystem.h"
#include "system/EnvironmentSystem.h"
#include "system/ObjectSystem.h"
#include "system/ParticleEmitterSystem.h"
#include "system/PointLightSystem.h"
#include "system/SkeletalAnimationSystem.h"
#include "system/UISystem.h"
#include "system/WidgetSystem.h"

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

namespace ui::editor
{
class Editor;
} // namespace ui::editor

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
                         asset::NcAsset* assetModule,
                         Registry* registry,
                         window::WindowImpl* window,
                         std::unique_ptr<ui::editor::Editor> editor) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                       Registry* registry,
                       asset::NcAsset* assetModule,
                       std::unique_ptr<IGraphics> graphics,
                       ShaderResourceBus&& shaderResourceBus,
                       window::WindowImpl* window,
                       std::unique_ptr<ui::editor::Editor> editor);

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
        asset::NcAsset* m_assetModule;
        std::unique_ptr<IGraphics> m_graphics;
        CameraSystem m_cameraSystem;
        EnvironmentSystem m_environmentSystem;
        ObjectSystem m_objectSystem;
        PointLightSystem m_pointLightSystem;
        ParticleEmitterSystem m_particleEmitterSystem;
        SkeletalAnimationSystem m_skeletalAnimationSystem;
        WidgetSystem m_widgetSystem;
        UISystem m_uiSystem;
    };
} // namespace graphics
} // namespace nc
