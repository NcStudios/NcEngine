#pragma once

#include "diligent/DiligentEngine.h"
#include "diligent/pass/MaterialPassBackend.h"
#include "diligent/pass/PostProcessPassBackend.h"
#include "diligent/pass/WireframePass.h"
#include "diligent/UIBackend.h"
#include "diligent/resource/ShaderBindings.h"
#include "frontend/GraphicsFrontend.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/graphics/NcGraphics.h"
#include "ncengine/module/ModuleProvider.h"

namespace nc
{
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
                       std::string_view shadersPath,
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
        auto IsPostProcessEffectEnabled(PostProcessEffectId effectId) const -> bool override;
        void SetPostProcessEffectEnabled(PostProcessEffectId effectId, bool enabled) override;
        auto GetPostProcessEffectProperties(PostProcessEffectId effectId,
                                            PostProcessPassFlag::type pass) const -> const PostProcessPassProperties& override;
        void SetPostProcessEffectProperties(PostProcessEffectId effectId,
                                            PostProcessPassFlag::type pass,
                                            const PostProcessPassProperties& properties) override;
        void OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render) override;
        void OnBeforeSceneLoad() override;
        void Clear() noexcept override;
        void Run();
        void OnResize(const Vector2& dimensions, bool isMinimized);

    private:
        ecs::Ecs m_world;
        DiligentEngine m_engine;
        ShaderBindings m_shaderBindings;
        UIBackend m_ui;
        MaterialPassBackend m_materialPassBackend;
        WireframePass m_wireframePass;
        PostProcessPassBackend m_postProcessPassBackend;
        GraphicsFrontend m_frontend;
        Connection m_onResizeConnection;
};
} // namespace graphics
} // namespace nc
