#pragma once

#include "diligent/DiligentEngine.h"
#include "diligent/pass/PassBackend.h"
#include "diligent/pass/PassManifest.h"
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
                       ecs::Ecs world,
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
        void Resize();
        ecs::Ecs m_world;
        DiligentEngine m_engine;
        ShaderBindings m_shaderBindings;
        UIBackend m_ui;
        PassManifest m_passManifest;
        PassBackend m_passBackend;
        GraphicsFrontend m_frontend;
        Connection m_onResizeConnection;
        Vector2 m_dimensions;
        bool m_resizeNeeded;
        uint32_t m_numSamples;
        bool m_isMinimized;
};
} // namespace graphics
} // namespace nc
