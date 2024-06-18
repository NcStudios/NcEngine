#pragma once

#include "IGraphics.h"
#include "graphics/shader_resource/ResourceInstances.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

#include <memory>

namespace nc
{
class Scene;

namespace window
{
class NcWindow;
} // namespace window

namespace graphics
{
struct ShaderResourceBus;

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
                       window::NcWindow& window);

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render) override;
        void Clear() noexcept override;
        void Run();
        void OnResize(const Vector2& dimensions, bool isMinimized);

    private:
        Registry* m_registry;
        std::unique_ptr<IGraphics> m_graphics;
        ShaderResourceBus m_shaderResourceBus;
        AssetResources m_assetResources;
        SystemResources m_systemResources;
        Connection<const Vector2&, bool> m_onResizeConnection;
    };
} // namespace graphics
} // namespace nc
