#pragma once

#include "AssetDispatch.h"
#include "diligent/DiligentEngine.h"
#include "diligent/TestPipeline.h"
#include "diligent/resource/ShaderBindings.h"
#include "frontend/GraphicsFrontend.h"
#include "ncengine/asset/AssetData.h"
#include "ncengine/asset/NcAsset.h"
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
class NcGraphicsImpl2 : public NcGraphics
{
    public:
        NcGraphicsImpl2(const config::GraphicsSettings& graphicsSettings,
                       const config::MemorySettings& memorySettings,
                       Registry* registry,
                       ModuleProvider modules,
                       SystemEvents& events,
                       window::NcWindow& window);

        ~NcGraphicsImpl2() noexcept;

        void SetCamera(Camera* camera) noexcept override;
        auto GetCamera() noexcept -> Camera* override;
        void SetUi(ui::IUI* ui) noexcept override;
        bool IsUiHovered() const noexcept override;
        void SetSkybox(const std::string& path) override;
        void ClearEnvironment() override;
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render) override;
        void Clear() noexcept override;
        void Run();
        void Update();
        void OnResize(const Vector2& dimensions, bool isMinimized);

    private:
        Registry* m_registry;
        DiligentEngine m_engine;
        ShaderBindings m_shaderBindings;
        AssetDispatch m_assetDispatch;
        TestPipeline m_testPipeline;
        GraphicsFrontend m_frontend;
        Connection m_onResizeConnection;
};
} // namespace graphics
} // namespace nc
