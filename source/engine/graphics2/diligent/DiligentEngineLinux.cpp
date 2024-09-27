#include "DiligentEngine.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_X11 1
#include <dlfcn.h>

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

auto CheckLibrary(const char* soName) -> bool
{
    void* handle = dlopen(soName, RTLD_LAZY);
    if (!handle) {
        return false;
    }
    dlclose(handle);
    return true;
}
} // anonymous namespace

namespace nc::graphics
{
auto GetSupportedRenderApiByPlatform(std::string_view targetApi) -> std::string_view
{
    auto preferredLinuxApiOrder = std::vector<std::string_view> 
    {
        api::Vulkan,
        api::OpenGL
    };

    if (targetApi == api::OpenGL)
    {
        RotateElementToBeginning(preferredLinuxApiOrder, api::OpenGL);
    }
    else if (targetApi != api::Vulkan)  // Vulkan already in front if targetApi is Vulkan
    {
        throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: vulkan, opengl", targetApi));
    }

    for (const auto& api : preferredLinuxApiOrder)
    {
        if (api == api::Vulkan)
        {
            if (CheckLibrary("libvulkan.so"))
                return api;
        }
        else if (api == api::OpenGL)
        {
            return api;
        }
    }
    throw nc::NcError("No supported API found from [vulkan, opengl]. Platform: Linux");
}

DiligentEngine::DiligentEngine(std::string_view renderApi, window::NcWindow& window_)
{
    using namespace Diligent;

    LinuxNativeWindow window;
    window.WindowId = static_cast<Diligent::Uint32>(glfwGetX11Window(window_.GetWindowHandle()));
    window.pDisplay = glfwGetX11Display();

    SwapChainDesc SCDesc;

    /* Initialize cross-api engine */
    if (renderApi == api::Vulkan)
    {
        #if EXPLICITLY_LOAD_ENGINE_VK_DLL
        auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
        #endif
        EngineVkCreateInfo engineCI;
        auto* pFactoryVk = GetEngineFactoryVk();
        pFactoryVk->CreateDeviceAndContextsVk(engineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, window, &m_pSwapChain);
        m_renderApi = renderApi;
    }
    else if (renderApi == api::OpenGL)
    {
        #if EXPLICITLY_LOAD_ENGINE_GL_DLL
        auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
        #endif
        auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
        EngineGLCreateInfo engineCI;
        glfwMakeContextCurrent(window_.GetWindowHandle());
        engineCI.Window = window;
        pFactoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
        m_renderApi = renderApi;
    }
    else
    {
        throw nc::NcError("Failed to initialize the rendering engine. Unsupported rendering API value given.");
    }
}

DiligentEngine::~DiligentEngine() noexcept
{
    if (m_pImmediateContext)
    {
        m_pImmediateContext->Flush();
    }
}
} // namespace nc::graphics
