#include "DiligentEngine.h"
#include "config/Config.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_X11 1

#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include <string_view>

namespace
{
template<typename T>
void RotateElementToBeginning(std::vector<T>& vectorToRotate, const T& elem)
{
    auto elemPos = std::ranges::find(vectorToRotate, elem);
    NC_ASSERT(elemPos != vectorToRotate.end(), fmt::format("{0} was not present in the vector.", elem));
    T element = *elemPos;
    vectorToRotate.erase(elemPos);
    vectorToRotate.insert(vectorToRotate.begin(), element);
}

void EnsureContextFlushed(Diligent::IDeviceContext* context)
{
    if (context)
        context->Flush();
}
} // anonymous namespace

namespace nc::graphics
{
DiligentEngine::DiligentEngine(const config::GraphicsSettings& graphicsSettings, window::NcWindow& window_)
{
    using namespace Diligent;

    LinuxNativeWindow window;
    window.WindowId = static_cast<Diligent::Uint32>(glfwGetX11Window(window_.GetWindowHandle()));
    window.pDisplay = glfwGetX11Display();

    const auto& renderApi = graphicsSettings.api;
    std::string errorMessage;

    auto preferredApiOrder = std::vector<std::string_view> 
    {
        api::Vulkan,
        api::OpenGL
    };

    if (renderApi == api::OpenGL)
    {
        RotateElementToBeginning(preferredApiOrder, api::OpenGL);
    }
    else if (renderApi != api::Vulkan)  // Vulkan already in front if renderApi is Vulkan
    {
        throw nc::NcError(fmt::format("API specified in the config: {0} is not in the list of potential APIs. Potential APIs: vulkan, opengl", renderApi));
    }

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

    if (m_renderApi == "" || errorMessage != "")
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
