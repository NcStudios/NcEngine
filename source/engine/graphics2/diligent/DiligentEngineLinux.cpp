#include "DiligentEngine.h"
#include "config/Config.h"
#include "DiligentUtils.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"

#define GLFW_EXPOSE_NATIVE_X11 1

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

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
DiligentEngine::DiligentEngine(const config::GraphicsSettings& graphicsSettings, GLFWwindow* window_, std::span<const std::string_view> supportedApis)
{
    using namespace Diligent;

    const std::string_view& renderApi = graphicsSettings.api;
    auto preferredApiOrder = std::vector<std::string_view>{};
    preferredApiOrder.reserve(2);
    std::ranges::copy(supportedApis, std::back_inserter(preferredApiOrder));
    RotateElementToBeginning(preferredApiOrder, renderApi);

    std::string errorMessage;
    LinuxNativeWindow window;
    window.WindowId = static_cast<Diligent::Uint32>(glfwGetX11Window(window_));
    window.pDisplay = glfwGetX11Display();
    SwapChainDesc SCDesc;

    /* Initialize the device and context. First try to init the preferred API. Fall back to others on failure. */
    for (const auto& api : preferredApiOrder)
    {
        if (api == api::Vulkan)
        {
            try
            {
                #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                    auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
                #endif

                EngineVkCreateInfo engineCI;
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
        else if (api == api::OpenGL)
        {
            try
            {
                #if EXPLICITLY_LOAD_ENGINE_GL_DLL
                    auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
                #endif

                auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
                EngineGLCreateInfo engineCI;
                glfwMakeContextCurrent(window_);
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
        throw nc::NcError(fmt::format("Failed to initialize the rendering engine. The given API and all fallback APIs failed to initialize. {0}", errorMessage));
    }
}

DiligentEngine::~DiligentEngine() noexcept
{
    EnsureContextFlushed(m_pImmediateContext);
}
} // namespace nc::graphics
