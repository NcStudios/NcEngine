#include "UIBackend.h"

namespace nc::graphics
{
UIBackend::UIBackend(Diligent::IRenderDevice& device,
                     const Diligent::SwapChainDesc& swapChainDesc,
                     GLFWwindow* window,
                     std::string_view api,
                     Signal<>& onFontUpdate)
    : m_imguiBackend{device, swapChainDesc, window, api},
      m_fontConnetion{onFontUpdate.Connect(this, &UIBackend::OnFontUpdate)}
{
}

void UIBackend::FrameBegin(Diligent::ISwapChain& swapChain)
{
    const auto& scDesc = swapChain.GetDesc();
    m_imguiBackend.NewFrame(scDesc.Width, scDesc.Height, scDesc.PreTransform);
}

void UIBackend::Render(Diligent::IDeviceContext& context)
{
    m_imguiBackend.Render(&context);
}

void UIBackend::OnFontUpdate()
{
    m_imguiBackend.UpdateFontsTexture();
}
} // namespace nc::graphics
