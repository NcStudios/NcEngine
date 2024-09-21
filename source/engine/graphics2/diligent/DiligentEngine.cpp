#include "DiligentEngine.h"
#include "ncengine/window/Window.h"
#include "ncutility/NcError.h"
#include "ncengine/utility/Log.h"

#if PLATFORM_WIN32
    #define GLFW_EXPOSE_NATIVE_WIN32 1
#elif PLATFORM_LINUX
    #define GLFW_EXPOSE_NATIVE_X11 1
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

// Diligent
#if PLATFORM_WIN32
    #if D3D11_SUPPORTED
        #include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
    #endif
    #if D3D12_SUPPORTED
        #include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
    #endif
#endif
#if GL_SUPPORTED
    #include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#endif
#if VULKAN_SUPPORTED
    #include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"
#endif
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"

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
        {
            RotateElementToBeginning(preferredWin32ApiOrder, Vulkan);
        }
        else if (targetApi == D3D11)
        {
            RotateElementToBeginning(preferredWin32ApiOrder, D3D11);
        }
        else if (targetApi == OpenGL)
        {
            RotateElementToBeginning(preferredWin32ApiOrder, OpenGL);
        }
        else if (targetApi != D3D12) // D3D12 already in front if targetApi is D3D12
        {
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: d3d12, vulkan, d3d11, opengl", targetApi));
        }
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
        {
            NC_LOG_WARNING("Target API of D3D(11,12) is not supported on Linux. Defaulting to Linux-supported API.");
        }
        else if (targetApi == OpenGL)
        {
            RotateElementToBeginning(preferredLinuxApiOrder, OpenGL);
        }
        else if (targetApi != Vulkan)  // Vulkan already in front if targetApi is Vulkan
        {
            throw nc::NcError(fmt::format("Target API of {0} is not in the list of potential APIs. Potential APIs: vulkan, opengl", targetApi));
        }

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
DiligentEngine::DiligentEngine(std::string_view targetApi, window::NcWindow& window_)
{
    using namespace Diligent;

    /* Initialize cross-platform window */
    #if PLATFORM_WIN32
        auto window = Win32NativeWindow{glfwGetWin32Window(window_.GetWindowHandle())};
    #elif PLATFORM_LINUX
        LinuxNativeWindow window;
        window.WindowId = static_cast<Diligent::Uint32>(glfwGetX11Window(window_.GetWindowHandle()));
        window.pDisplay = glfwGetX11Display();
    #endif

    SwapChainDesc SCDesc;
    auto supportedDeviceTypeByPlatform = GetSupportedRenderDeviceTypeByPlatform(targetApi);

    /* Initialize cross-api engine */
    switch (supportedDeviceTypeByPlatform)
    {
        #if PLATFORM_WIN32
            case RENDER_DEVICE_TYPE_D3D12:
            {
                #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                    auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
                #endif
                EngineD3D12CreateInfo engineCI;
                auto* pFactoryD3D12 = GetEngineFactoryD3D12();
                pFactoryD3D12->CreateDeviceAndContextsD3D12(engineCI, &m_pDevice, &m_pImmediateContext);
                pFactoryD3D12->CreateSwapChainD3D12(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);
                break;
            }
        #endif
        case RENDER_DEVICE_TYPE_VULKAN:
        {
            #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                auto* GetEngineFactoryVk = LoadGraphicsEngineVk();
            #endif
            EngineVkCreateInfo engineCI;
            auto* pFactoryVk = GetEngineFactoryVk();
            pFactoryVk->CreateDeviceAndContextsVk(engineCI, &m_pDevice, &m_pImmediateContext);
            pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, window, &m_pSwapChain);
            break;
        }
        #if PLATFORM_WIN32
            case RENDER_DEVICE_TYPE_D3D11:
            {
                #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                    auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
                #endif
                EngineD3D11CreateInfo engineCI;
                auto* pFactoryD3D11 = GetEngineFactoryD3D11();
                pFactoryD3D11->CreateDeviceAndContextsD3D11(engineCI, &m_pDevice, &m_pImmediateContext);
                pFactoryD3D11->CreateSwapChainD3D11(m_pDevice, m_pImmediateContext, SCDesc, FullScreenModeDesc{}, window, &m_pSwapChain);
                break;
            }
        #endif
        case RENDER_DEVICE_TYPE_GL:
        {
            #if EXPLICITLY_LOAD_ENGINE_VK_DLL
                auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
            #endif
            auto* pFactoryOpenGL = GetEngineFactoryOpenGL();
            glfwMakeContextCurrent(window_.GetWindowHandle());
            EngineGLCreateInfo engineCI;
            engineCI.Window = window;
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
