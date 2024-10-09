#include "DiligentEngine.h"
#include "config/Config.h"
#include "DiligentUtils.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1
#ifdef GetObject
    #undef GetObject
#endif
#ifdef CreateWindow
    #undef CreateWindow
#endif

#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <ranges>
#include <string_view>

namespace
{
void EnsureContextFlushed(Diligent::IDeviceContext* context)
{
    if (context)
        context->Flush();
}
} // anonymous namespace

namespace nc::graphics
{
DiligentEngine::DiligentEngine(const config::GraphicsSettings& graphicsSettings, window::NcWindow& window_, std::span<const std::string_view> supportedApis)
{
    using namespace Diligent;

    const std::string_view& renderApi = graphicsSettings.api;
    auto preferredApiOrder = std::vector<std::string_view>{};
    preferredApiOrder.reserve(4);
    std::ranges::copy(supportedApis, std::back_inserter(preferredApiOrder));
    RotateElementToBeginning(preferredApiOrder, renderApi);

    std::string errorMessage;
    auto window = Win32NativeWindow{glfwGetWin32Window(window_.GetWindowHandle())};
    SwapChainDesc SCDesc;

    /* Initialize the device and context. First try to init the preferred API. Fall back to others on failure. */
    for (const auto& api : preferredApiOrder)
    {
        if (api == api::D3D12)
        {
            try
            {
                #if ENGINE_DLL
                    auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
                #endif

                EngineD3D12CreateInfo engineCI;
                auto* pFactoryD3D12 = GetEngineFactoryD3D12();
                pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);

                if (!graphicsSettings.isHeadless)
                    pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);

                m_renderApi = api;
                NC_LOG_TRACE("Successfully initialized the D3D12 rendering engine.");
                break;
            }
            catch (const std::runtime_error& e)
            {
                EnsureContextFlushed(m_pImmediateContext);
                NC_LOG_WARNING("Failed to initialize D3D12.");
                errorMessage = fmt::format("{0} Failed to initialize D3D12: {1} \n", errorMessage, e.what());
            }
        }
        else if (api == api::Vulkan)
        {
            try
            {
                #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                    auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
                #endif
                EngineVkCreateInfo engineCI;
                engineCI.Features.BindlessResources = Diligent::DEVICE_FEATURE_STATE_ENABLED;
                auto* pFactoryVk = GetEngineFactoryVk();
                pFactoryVk->CreateDeviceAndContextsVk(engineCI, &m_pDevice, &m_pImmediateContext);

                if (!graphicsSettings.isHeadless)
                    pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, window, &m_pSwapChain);

                m_renderApi = api;
                NC_LOG_TRACE("Successfully initialized the Vulkan rendering engine.");
                break;
            }
            catch (const std::runtime_error& e)
            {
                EnsureContextFlushed(m_pImmediateContext);
                NC_LOG_WARNING("Failed to initialize Vulkan.");
                errorMessage = fmt::format("{0} Failed to initialize Vulkan: {1} \n", errorMessage, e.what());
            }
        }
        else if (api == api::D3D11)
        {
            try
            {
                #if ENGINE_DLL
                    auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
                #endif

                EngineD3D11CreateInfo engineCI;
                auto* pFactoryD3D11 = GetEngineFactoryD3D11();
                pFactoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &m_pDevice, &m_pImmediateContext);

                if (!graphicsSettings.isHeadless)
                    pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);

                m_renderApi = api;
                NC_LOG_TRACE("Successfully initialized the D3D11 rendering engine.");
                break;
            }
            catch (const std::runtime_error& e)
            {
                EnsureContextFlushed(m_pImmediateContext);
                NC_LOG_WARNING("Failed to initialize D3D11.");
                errorMessage = fmt::format("{0} Failed to initialize D3D11: {1} \n", errorMessage, e.what());
            }
        }
        else if (api == api::OpenGL)
        {
            try
            {
                #if EXPLICITLY_LOAD_ENGINE_GL_DLL
                    auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
                #endif

                auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
                EngineGLCreateInfo engineCI;
                glfwMakeContextCurrent(window_.GetWindowHandle());
                engineCI.Window = window;

                if (!graphicsSettings.isHeadless)
                    pFactoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);

                m_renderApi = api;
                NC_LOG_TRACE("Successfully initialized the OpenGL rendering engine.");
                break;
            }
            catch (const std::runtime_error& e)
            {
                EnsureContextFlushed(m_pImmediateContext);
                NC_LOG_WARNING("Failed to initialize OpenGL.");
                errorMessage = fmt::format("{0} Failed to initialize OpenGL: {1} \n", errorMessage, e.what());
            }
        }
    }
    
    if (m_renderApi == "")
    {
        EnsureContextFlushed(m_pImmediateContext);
        throw nc::NcError(fmt::format("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize. \n {0}", errorMessage));
    }
}

DiligentEngine::~DiligentEngine() noexcept
{
    EnsureContextFlushed(m_pImmediateContext);
}
} // namespace nc::graphics
