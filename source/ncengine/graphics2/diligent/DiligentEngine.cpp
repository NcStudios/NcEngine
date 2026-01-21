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

auto QueryDeviceCapability(Diligent::IRenderDevice* device, Diligent::ISwapChain* swapChain, const nc::graphics::DeviceCapability& requestedCaps)
{
    auto supportedDeviceCaps = nc::graphics::DeviceCapability{};

    // Supported MSAA sample count
    const auto& colorFormat = device->GetTextureFormatInfoExt(swapChain->GetDesc().ColorBufferFormat);
    const auto& depthFormat = device->GetTextureFormatInfoExt(swapChain->GetDesc().DepthBufferFormat);

    auto requestedMsaaCount = requestedCaps.msaaSampleCount;
    auto supportedMsaaCount = colorFormat.SampleCounts & depthFormat.SampleCounts;

    // We only support a max of 8 samples because of texture formats used.
    if (requestedMsaaCount > 7 && supportedMsaaCount & Diligent::SAMPLE_COUNT_8)
    {
        supportedDeviceCaps.msaaSampleCount = 8;
    }
    else if (requestedMsaaCount > 3 && supportedMsaaCount & Diligent::SAMPLE_COUNT_4)
    {
        supportedDeviceCaps.msaaSampleCount = 4;
    }
    else if (requestedMsaaCount > 1 && supportedMsaaCount & Diligent::SAMPLE_COUNT_2)
    {
        supportedDeviceCaps.msaaSampleCount = 2;
    }
    else
    {
        if (requestedMsaaCount > 1)
        {
            NC_LOG_TRACE("Multisampling not supported on this device.");
        }
        supportedDeviceCaps.msaaSampleCount = 1;
    }

    return supportedDeviceCaps;
}
} // anonymous namespace

namespace nc::graphics
{
DiligentEngine::DiligentEngine(const Diligent::EngineCreateInfo& engineCreateInfo,
                               DeviceCapability requestedCaps,
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
    engineCI.Features.ShaderResourceRuntimeArrays = DEVICE_FEATURE_STATE_ENABLED;
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

    m_deviceCapability = QueryDeviceCapability(m_pDevice, m_pSwapChain, requestedCaps);

    m_shaderFactory = MakeShaderFactory(*pFactoryVk, *m_pDevice, shadersPath);
    NC_LOG_TRACE("Successfully initialized rendering engine.");
}

DiligentEngine::~DiligentEngine() noexcept
{
    EnsureContextFlushed(m_pImmediateContext);
}
} // namespace nc::graphics
