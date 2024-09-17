#include "DiligentEngine.h"

#if PLATFORM_WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif PLATFORM_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
    #include <GL/glx.h>
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <GL/gl.h>

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
template<typename T>
void RotateElementToBeginning(std::vector<T>& vectorToRotate, const T& elem)
{
    auto elemPos = std::ranges::find(vectorToRotate, elem);
    NC_ASSERT(elemPos != vectorToRotate.end(), fmt::format("{0} was not present in the vector.", elem));
    T element = *elemPos;
    vectorToRotate.erase(elemPos);
    vectorToRotate.insert(vectorToRotate.begin(), element);
}

auto GetSupportedRenderDeviceTypeByPlatform(std::string_view targetApi) -> Diligent::RENDER_DEVICE_TYPE
{
    constexpr std::string_view D3D12  = std::string_view("d3d12");
    constexpr std::string_view Vulkan = std::string_view("vulkan");
    constexpr std::string_view D3D11  = std::string_view("d3d11");
    constexpr std::string_view OpenGL = std::string_view("opengl");

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
        if (targetApi == Vulkan)
            RotateElementToBeginning(preferredWin32ApiOrder, Vulkan);
        else if (targetApi == D3D11)
            RotateElementToBeginning(preferredWin32ApiOrder, D3D11);
        else if (targetApi == OpenGL)
            RotateElementToBeginning(preferredWin32ApiOrder, OpenGL);
        else if (targetApi != D3D12) // D3D12 already in front if targetApi is D3D12
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: d3d12, vulkan, d3d11, opengl", targetApi));

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
        throw nc::NcError("No supported API found from [d3d12, vulkan, d3d11, opengl]. Platform: Win32");

    #elif PLATFORM_LINUX
        if (targetApi == D3D12 || targetApi == D3D11)
            NC_LOG_WARNING(fmt::format("Target API of {0} is not supported on Linux. Defaulting to Linux-supported API.", targetApi));
        else if (targetApi == OpenGL)
            RotateElementToBeginning(preferredLinuxApiOrder, OpenGL);
        else if (targetApi != Vulkan)  // Vulkan already in front if targetApi is Vulkan
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: vulkan, opengl", targetApi));

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
        throw nc::NcError("No supported API found from [vulkan, opengl]. Platform: Linux");
    #else
        #error "Unsupported platform detected. NcEngine only supports Win32 and Linux."
    #endif
}
} // anonymous namespace

namespace nc::graphics
{
DiligentEngine::DiligentEngine(std::string_view targetApi, window::NcWindow& window)
{
    using namespace Diligent;

    /* Initialize Diligent Engine */
    SwapChainDesc SCDesc;

    auto supportedDeviceTypeByPlatform = GetSupportedRenderDeviceTypeByPlatform(graphicsSettings.targetApi);

    switch (supportedDeviceTypeByPlatform)
    {
        case RENDER_DEVICE_TYPE_D3D12:
        {
            auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
            EngineD3D12CreateInfo engineCI;
            auto* pFactoryD3D12 = GetEngineFactoryD3D12();
            pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);
            auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
            Win32NativeWindow win32Window{win32Handle};
            pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, win32Window, &m_pSwapChain);
            break;
        }
        case RENDER_DEVICE_TYPE_VULKAN:
        {
            auto GetEngineFactoryVk = LoadGraphicsEngineVk();
            EngineVkCreateInfo engineCI;
            auto* pFactoryVk = GetEngineFactoryVk();
            pFactoryVk->CreateDeviceAndContextsVk(engineCI, &m_pDevice, &m_pImmediateContext);
            auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
            if (!m_pSwapChain && win32Handle != nullptr)
            {
                Win32NativeWindow win32Window{win32Handle};
                pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, win32Window, &m_pSwapChain);
            }
            break;
        }
        case RENDER_DEVICE_TYPE_D3D11:
        {
            auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
            EngineD3D11CreateInfo engineCI;
            auto* pFactoryD3D11 = GetEngineFactoryD3D11();
            pFactoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &m_pDevice, &m_pImmediateContext);
            auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
            Win32NativeWindow win32Window{win32Handle};
            pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, win32Window, &m_pSwapChain);
            break;
        }
        case RENDER_DEVICE_TYPE_GL:
        {
            auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
            auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
            EngineGLCreateInfo engineCI;
            auto win32Handle = glfwGetWin32Window(window.GetWindowHandle());
            Win32NativeWindow win32Window{win32Handle};
            engineCI.Window.hWnd = win32Handle;
            pFactoryOpenGL->CreateDeviceAndSwapChainGL(engineCI, &m_pDevice, &m_pImmediateContext, SCDesc, &m_pSwapChain);
            break;
        }
        default:
            throw nc::NcError("Failed to initialize the render device. Unsupported device type.");
    }
}

DiligentEngine::~DiligentEngine() noexcept
{
    m_pImmediateContext->Flush();
}
} // namespace nc::graphics
