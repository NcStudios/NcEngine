#include "NcGraphics3Impl.h"
#include "graphics/PerFrameRenderState.h"
#include "graphics/shader_resource/ShaderResourceBus.h"
#include "ncengine/asset/NcAsset.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/DebugRendering.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/ecs/View.h"
#include "ncengine/graphics/WireframeRenderer.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"

#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"

#include "imgui/imgui.h"
#include "optick.h"

namespace
{
    struct NcGraphicsStub : nc::graphics::NcGraphics
    {
        NcGraphicsStub(nc::Registry*)
        {
            // client app may still make imgui calls (font loading, etc.), so we need a context
            ImGui::CreateContext();
        }

        ~NcGraphicsStub() noexcept
        {
            ImGui::DestroyContext();
        }

        void OnBuildTaskGraph(nc::task::UpdateTasks& update, nc::task::RenderTasks& render)
        {
            update.Add(
                nc::update_task_id::ParticleEmitterUpdate,
                "ParticleEmitterUpdate(stub)",
                []{}
            );

            update.Add(
                nc::update_task_id::ParticleEmitterSync,
                "ParticleEmitterSync(stub)",
                []{},
                {nc::update_task_id::CommitStagedChanges}
            );

            render.Add(
                nc::render_task_id::Render,
                "Render(stub)",
                []{}
            );
        }

        void SetCamera(nc::graphics::Camera*) noexcept override {}
        auto GetCamera() noexcept -> nc::graphics::Camera* override { return nullptr; }
        void SetUi(nc::ui::IUI*) noexcept override {}
        bool IsUiHovered() const noexcept override { return false; }
        void SetSkybox(const std::string&) override {}
        void ClearEnvironment() override {}
    };
} // anonymous namespace

namespace nc::graphics
{
    auto BuildGraphicsModule(const config::ProjectSettings& projectSettings,
                             const config::GraphicsSettings& graphicsSettings,
                             const config::MemorySettings& memorySettings,
                             ModuleProvider modules,
                             Registry* registry,
                             SystemEvents& events) -> std::unique_ptr<NcGraphics>
    {
        (void)projectSettings;
        (void)memorySettings;
        (void)events;

        if (graphicsSettings.enabled)
        {
            auto ncAsset = modules.Get<asset::NcAsset>();
            auto ncWindow = modules.Get<window::NcWindow>();
            NC_ASSERT(ncAsset, "NcGraphics requires NcAsset to be registered before it.");
            NC_ASSERT(ncWindow, "NcGraphics requires NcWindow to be registered before it.");
            NC_ASSERT(modules.Get<NcScene>(), "NcGraphics requires NcScene to be registered before it.");
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub>(registry);
    }

    NcGraphics3Impl::NcGraphics3Impl(const config::GraphicsSettings& graphicsSettings,
                                   const config::MemorySettings& memorySettings,
                                   Registry* registry,
                                   ModuleProvider modules,
                                   SystemEvents& events,
                                   window::NcWindow& window)
        : m_registry{registry},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphics3Impl::OnResize)}
    {
        (void)graphicsSettings;
        (void)memorySettings;
        (void)modules;
        (void)events;



    }

    void NcGraphics3Impl::SetCamera(Camera* camera) noexcept
    {
        (void)camera;
    }

    auto NcGraphics3Impl::GetCamera() noexcept -> Camera*
    {
        return nullptr;
    }

    void NcGraphics3Impl::SetUi(ui::IUI* ui) noexcept
    {
        (void)ui;
    }

    bool NcGraphics3Impl::IsUiHovered() const noexcept
    {
        return false;
    }

    void NcGraphics3Impl::SetSkybox(const std::string& path)
    {
        (void)path;
    }

    void NcGraphics3Impl::ClearEnvironment()
    {
    }

    void NcGraphics3Impl::Clear() noexcept
    {
    }

    void NcGraphics3Impl::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
    {
        (void)update;

        NC_LOG_TRACE("Building NcGraphics Tasks");
        render.Add(
            render_task_id::Render,
            "Render",
            [this]{ Run(); }
        );
    }

    void NcGraphics3Impl::Run()
    {
        OPTICK_CATEGORY("Render", Optick::Category::Rendering);
    }

    void NcGraphics3Impl::OnResize(const Vector2& dimensions, bool isMinimized)
    {
        (void)dimensions;
        (void)isMinimized;
    }
} // namespace nc::graphics
