#pragma once

#include "IGraphics.h"
#include "graphics/shader_resource/ResourceInstances.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

#include <memory>

namespace nc
{
struct SystemEvents;
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
                         SystemEvents& events,
                         window::WindowImpl* window) -> std::unique_ptr<NcGraphics>;

class NcGraphicsImpl : public NcGraphics
{
    public:
        NcGraphicsImpl(const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       Registry* registry,
                       ModuleProvider modules,
                       SystemEvents& events,
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
        PostProcessResources m_postProcessResources;
        AssetResources m_assetResources;
        SystemResources m_systemResources;
    };
} // namespace graphics
} // namespace nc
