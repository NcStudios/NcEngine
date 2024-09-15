#include "NcGraphicsImpl2.h"
#include "GraphicsTypes.h"
#include "ncengine/config/Config.h"
#include "ncengine/ecs/Ecs.h"
#include "ncengine/scene/NcScene.h"
#include "ncengine/task/TaskGraph.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "config/Config.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui/imgui.h"
#include "optick.h"
#include "DirectXMath.h"

// Diligent
#include "EngineFactoryD3D11.h"
#include "EngineFactoryD3D12.h"
#include "EngineFactoryOpenGL.h"
#include "EngineFactoryVk.h"
#include "DeviceContext.h"
#include "SwapChain.h"
#include "MapHelper.hpp"
#include "GraphicsUtilities.h"
#include "TextureUtilities.h"

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

auto GetSupportedRenderDeviceType(std::string_view targetApi, nc::graphics::Platform platform) -> Diligent::RENDER_DEVICE_TYPE
{
    bool vulkanSupported = false;
    bool d3d12Supported = false;
    bool d3d11Supported = false;
    bool openGLSupported = false;

    #if VULKAN_SUPPORTED
        vulkanSupported = true;
    #endif
    #if D3D12_SUPPORTED
        d3d12Supported = true;
    #endif
    #if D3D11_SUPPORTED
        d3d11Supported = true;
    #endif
    #if GL_SUPPORTED
        openGLSupported = true;
    #endif

    // vulkan, d3d12, d3d11, opengl
    if (targetApi == "vulkan")
    {
        switch (platform)
        {
            case nc::graphics::Platform::Win32:
            {
                if (vulkanSupported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_VULKAN;
                if (d3d12Supported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D12;
                if (d3d11Supported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D11;
                if  (openGLSupported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;
                throw nc::NcError("No supported render device found from [Vulkan, D3D12, D3D11, OpenGL]. Platform: Win32");
            }
            case nc::graphics::Platform::Linux:
                if (vulkanSupported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_VULKAN;
                if  (openGLSupported)
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;
                throw nc::NcError("No supported render device found from [Vulkan, OpenGL]. Platform: Linux.");
        }
        std::unreachable();
        throw nc::NcError()
    }
    if (targetApi == "vulkan")
    {
        if (vulkanSupported)
            return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_VULKAN;

        if (platform == nc::graphics::Platform::Win32)
        {
            if (d3d12Supported)
                return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D12;

            if (d3d11Supported)
                return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D11;

            if  (openGLSupported)
                return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;

            throw nc::NcError("No supported render device found from [Vulkan, D3D12, D3D11, OpenGL]. Platform: Win32");
        }
        if (platform == nc::graphics::Platform::Linux)
        {
            if  (openGLSupported)
                return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;
            throw nc::NcError("No supported render device found from [Vulkan, OpenGL]. Platform: Linux.");
        }
    }
}
} // anonymous namespace

namespace nc::graphics
{
#define NC_USE_DILIGENT 1 // TODO: REMOVE
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
          m_onResizeConnection{window.OnResize().Connect(this, &NcGraphicsImpl2::OnResize)}
{
    (void)graphicsSettings;
    (void)memorySettings;
    (void)modules;
    (void)events;

    using namespace Diligent;

    ImGui::CreateContext();

    /* Initialize Diligent Engine */
    SwapChainDesc SCDesc;
    auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
    EngineD3D12CreateInfo engineCI;
    auto* pFactoryD3D12 = GetEngineFactoryD3D12();
    pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);
    auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
    Win32NativeWindow win32Window{win32Handle};
    pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, win32Window, &m_pSwapChain);
}

NcGraphicsImpl2::~NcGraphicsImpl2()
{
    m_pImmediateContext->Flush();
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

    update.Add(
        update_task_id::UpdateRenderData,
        "Update",
        [this]{ Update(); }
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
    OPTICK_CATEGORY("Render", Optick::Category::Rendering);
    using namespace Diligent;
}

void NcGraphicsImpl2::OnResize(const Vector2& dimensions, bool isMinimized)
{
    (void)isMinimized;

    if (m_pSwapChain)
        m_pSwapChain->Resize(static_cast<uint32_t>(dimensions.x), static_cast<uint32_t>(dimensions.y));
}
} // namespace nc::graphics
