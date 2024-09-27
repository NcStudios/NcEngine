#include "DiligentEngine.h"
#include "ncengine/utility/Log.h"
#include "ncengine/window/Window.h"
#include "ncutility/NcError.h"

#define GLFW_EXPOSE_NATIVE_WIN32 1
#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#ifdef GetObject
    #undef GetObject
#endif
#ifdef CreateWindow
    #undef CreateWindow
#endif

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

auto CheckLibrary(char const* dllName) -> bool
{
    auto hModule = LoadLibrary(dllName);
    if (hModule == NULL) {
        return false;
    }
    FreeLibrary(hModule);
    return true;
}
} // anonymous namespace

namespace nc::graphics
{
auto GetSupportedRenderApiByPlatform(std::string_view targetApi) -> std::string_view
{
    auto preferredWin32ApiOrder = std::vector<std::string_view> 
    {
        api::D3D12,
        api::Vulkan,
        api::D3D11,
        api::OpenGL
    };

    if (targetApi == api::Vulkan)
    {
        RotateElementToBeginning(preferredWin32ApiOrder, api::Vulkan);
    }
    else if (targetApi == api::D3D11)
    {
        RotateElementToBeginning(preferredWin32ApiOrder, api::D3D11);
    }
    else if (targetApi == api::OpenGL)
    {
        RotateElementToBeginning(preferredWin32ApiOrder, api::OpenGL);
    }
    else if (targetApi != api::D3D12) // D3D12 already in front if targetApi is D3D12
    {
        throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: d3d12, vulkan, d3d11, opengl", targetApi));
    }

    for (const auto& api : preferredWin32ApiOrder)
    {
        if (api == api::D3D12)
        {
            if (CheckLibrary("d3d12.dll"))
                return api;
        }
        else if (api == api::Vulkan)
        {
            if (CheckLibrary("vulkan-1.dll"))
                return api;
        }
        else if (api == api::D3D11)
        {
            if (CheckLibrary("d3d11.dll"))
                return api;
        }
        else if (api == api::OpenGL)
        {
            return api;
        }
    }

    throw nc::NcError("No supported API found from [d3d12, vulkan, d3d11, opengl]. Platform: Win32");
}

DiligentEngine::DiligentEngine(std::string_view renderApi, window::NcWindow& window_)
{
    using namespace Diligent;

    auto window = Win32NativeWindow{glfwGetWin32Window(window_.GetWindowHandle())};

    SwapChainDesc SCDesc;

    /* Initialize cross-api engine */
    if (renderApi == api::D3D12)
    {
        #if ENGINE_DLL
        auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
        #endif
        EngineD3D12CreateInfo engineCI;
        auto* pFactoryD3D12 = GetEngineFactoryD3D12();
        pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);
        m_renderApi = renderApi;
    }
    else if (renderApi == api::Vulkan)
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
    else if (renderApi == api::D3D11)
    {
        #if ENGINE_DLL
        auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
        #endif
        EngineD3D11CreateInfo engineCI;
        auto* pFactoryD3D11 = GetEngineFactoryD3D11();
        pFactoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &m_pDevice, &m_pImmediateContext);
        pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);
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
