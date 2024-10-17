#include "DiligentEngine.h"
#include "DiligentUtils.h"
#include "ShaderFactory.h"
#include "ncengine/config/Config.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"

#define GLFW_EXPOSE_NATIVE_X11 1

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
DiligentEngine::DiligentEngine(const config::GraphicsSettings& graphicsSettings,
                               const Diligent::EngineCreateInfo& engineCreateInfo,
                               GLFWwindow* window_,
                               std::span<const std::string_view> supportedApis,
                               Diligent::DebugMessageCallbackType logCallback)
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

                auto* pFactoryVk = GetEngineFactoryVk();
                if (logCallback)
                {
                    pFactoryVk->SetMessageCallback(logCallback);
                }

                auto engineCI = EngineVkCreateInfo{engineCreateInfo};
                pFactoryVk->CreateDeviceAndContextsVk(engineCI, &m_pDevice, &m_pImmediateContext);

                if (!m_pDevice || !m_pImmediateContext)
                {
                    throw nc::NcError("Failed to create the Vulkan device or context.");
                }

                if (!graphicsSettings.isHeadless)
                    pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, window, &m_pSwapChain);

                if (!graphicsSettings.isHeadless && !m_pSwapChain)
                {
                    throw nc::NcError("Failed to create the Vulkan swapchain.");
                }

                m_shaderFactory = MakeShaderFactory(*pFactoryVk, *m_pDevice);
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
