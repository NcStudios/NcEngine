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

template<typename T>
void RotateElementToBeginning(std::vector<T>& vectorToRotate, const T& elem)
{
    auto elemPos = std::ranges::find(vectorToRotate, elem);
    NC_ASSERT(elemPos != vectorToRotate.end(), fmt::format("{0} was not present in the vector.", elem));
    // Store the element before modifying the vector
    T element = *elemPos;

    // Erase the element from its current position
    vectorToRotate.erase(elemPos);

    // Insert the element at the beginning
    vectorToRotate.insert(vectorToRotate.begin(), element);
}

auto GetSupportedRenderDeviceTypeByPlatform(std::string_view targetApi) -> Diligent::RENDER_DEVICE_TYPE
{
    constexpr std::string_view D3D12 = std::string_view("d3d12");
    constexpr std::string_view Vulkan = std::string_view("vulkan");
    constexpr std::string_view D3D11 = std::string_view("d3d11");
    constexpr std::string_view OpenGL = std::string_view("openGL");

    auto preferredWin32ApiOrder = std::vector<std::string_view> 
    {
        D3D12,
        Vulkan,
        D3D11,
        OpenGL
    };

    auto preferredLinuxApiOrder = std::vector<std::string_view> 
    {
        Vulkan,
        OpenGL
    };

    #if PLATFORM_WIN32
        // D3D12 already in front if targetApi is D3D12
        if (targetApi == Vulkan)
            RotateElementToBeginning(preferredWin32ApiOrder, Vulkan);
        else if (targetApi == D3D11)
            RotateElementToBeginning(preferredWin32ApiOrder, D3D11);
        else if (targetApi == OpenGL)
            RotateElementToBeginning(preferredWin32ApiOrder, OpenGL);
        else
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: d3d12, vulkan, d3d11, openGL", targetApi));

        for (const auto& api : preferredWin32ApiOrder)
        {
            if (api == D3D12)
            {
                #if D3D12_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D12;
                #endif
            }
            else if (api == Vulkan)
            {
                #if VULKAN_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_VULKAN;
                #endif
            }
            else if (api == D3D11)
            {
                #if D3D11_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_D3D11;
                #endif
            }
            else if (api == OpenGL)
            {
                #if GL_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;
                #endif
            }
        }
        throw nc::NcError("No supported API found from [d3d12, vulkan, d3d11, openGL]. Platform: Win32");

    #elif PLATFORM_LINUX
        // Vulkan already in front if targetApi is Vulkan
        if (targetApi == D3D12 || targetApi == D3D11)
            NC_LOG_WARNING(fmt::format("Target API of {0} is not supported on Linux. Defaulting to Linux-supported API.", targetApi));
        else if (targetApi == OpenGL)
            RotateElementToBeginning(preferredLinuxApiOrder, OpenGL);
        else
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: vulkan, openGL", targetApi));

        for (const auto& api : preferredLinuxApiOrder)
        {
            if (api == Vulkan)
            {
                #if VULKAN_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_VULKAN;
                #endif
            }
            else if (api == OpenGL)
            {
                #if GL_SUPPORTED
                    return Diligent::RENDER_DEVICE_TYPE::RENDER_DEVICE_TYPE_GL;
                #endif
            }
        }
        throw nc::NcError("No supported API found from [vulkan, openGL]. Platform: Linux");
    #else
        #error "Unsupported platform detected. NcEngine only supports Win32 and Linux."
    #endif
}
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

    auto supportedDeviceTypeByPlatform = GetSupportedRenderDeviceTypeByPlatform(graphicsSettings.targetApi);

    switch (supportedDeviceTypeByPlatform)
    {
        case RENDER_DEVICE_TYPE_VULKAN:
        {
            
        }
    }

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
