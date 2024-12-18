#include "DiligentEngine.h"
#include "NativeWindow.h"
#include "ncengine/utility/Log.h"

#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

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
DiligentEngine::DiligentEngine(const Diligent::EngineCreateInfo& engineCreateInfo,
                               GLFWwindow* windowHandle,
                               std::string_view shadersPath,
                               Diligent::DebugMessageCallbackType logCallback)
{
    using namespace Diligent;

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
        EnsureContextFlushed(m_pImmediateContext);
        throw nc::NcError("Failed to create device or context.");
    }

    auto window = GetNativeWindow(windowHandle);
    auto SCDesc = SwapChainDesc{};
    pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc, window, &m_pSwapChain);
    if (!m_pSwapChain)
    {
        EnsureContextFlushed(m_pImmediateContext);
        throw nc::NcError("Failed to create swapchain.");
    }

    m_shaderFactory = MakeShaderFactory(*pFactoryVk, *m_pDevice, shadersPath);
    NC_LOG_TRACE("Successfully initialized rendering engine.");
}

DiligentEngine::~DiligentEngine() noexcept
{
    EnsureContextFlushed(m_pImmediateContext);
}
} // namespace nc::graphics
