#include "NcGraphicsImpl2.h"
#include "ncengine/config/Config.h"
#include "ncengine/debug/Profile.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "config/Config.h"
#include "window/NcWindowImpl.h"

#include "imgui/imgui.h"
#include "DirectXMath.h"

namespace
{
struct NcGraphicsStub2 : nc::graphics::NcGraphics
{
    NcGraphicsStub2(nc::Registry*)
    {
        // client app may still make imgui calls (font loading, etc.), so we need a context
        ImGui::CreateContext();
    }

    ~NcGraphicsStub2() noexcept
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
#ifdef NC_USE_DILIGENT
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

            ncWindow->SetWindow(window::WindowInfo
            {
                .dimensions = Vector2{static_cast<float>(graphicsSettings.screenWidth), static_cast<float>(graphicsSettings.screenHeight)},
                .isGL = graphicsSettings.api == api::OpenGL,
                .isHeadless = graphicsSettings.isHeadless,
                .useNativeResolution = graphicsSettings.useNativeResolution,
                .launchInFullScreen = graphicsSettings.launchInFullscreen,
                .isResizable = false
            });

            NC_LOG_TRACE("Building NcGraphics module");
            return std::make_unique<NcGraphicsImpl2>(graphicsSettings, memorySettings, registry, modules, events, *ncWindow);
        }

        NC_LOG_TRACE("Graphics disabled - building NcGraphics stub");
        return std::make_unique<NcGraphicsStub2>(registry);
    }
#endif

NcGraphicsImpl2::NcGraphicsImpl2(const config::GraphicsSettings& graphicsSettings,
                                 const config::MemorySettings& memorySettings,
                                 Registry* registry,
                                 ModuleProvider modules,
                                 SystemEvents& events,
                                 window::NcWindow& window)
        : m_registry{registry},
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl2::OnResize)},
          m_engineCreateInfo{},
          m_engine{graphicsSettings, m_engineCreateInfo, window.GetWindowHandle(), GetSupportedApis()}
{
    (void)graphicsSettings;
    (void)memorySettings;
    (void)modules;
    (void)events;

    ImGui::CreateContext();
}

NcGraphicsImpl2::~NcGraphicsImpl2()
{
}

void NcGraphicsImpl2::SetCamera(Camera* camera) noexcept
    {
        (void)camera;
    }

auto NcGraphicsImpl2::GetCamera() noexcept -> Camera*
{
    return nullptr;
}

void NcGraphicsImpl2::SetUi(ui::IUI* ui) noexcept
{
    (void)ui;
}

bool NcGraphicsImpl2::IsUiHovered() const noexcept
{
    return false;
}

void NcGraphicsImpl2::SetSkybox(const std::string& path)
{
    (void)path;
}

void NcGraphicsImpl2::ClearEnvironment()
{
}

void NcGraphicsImpl2::Clear() noexcept
{
}

void NcGraphicsImpl2::OnBuildTaskGraph(task::UpdateTasks& update, task::RenderTasks& render)
{
    NC_LOG_TRACE("Building NcGraphics Tasks");

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
        render_task_id::Render,
        "Render",
        [this]{ Run(); }
    );
}

void NcGraphicsImpl2::Update()
{
}

void NcGraphicsImpl2::Run()
{
    NC_PROFILE_TASK("Render", Optick::Category::Rendering);
}

void NcGraphicsImpl2::OnResize(const Vector2& dimensions, bool isMinimized)
{
    (void)isMinimized;
    m_engine.SwapChain()->Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
}
} // namespace nc::graphics
