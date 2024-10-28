#pragma once

#include "ImGuiImplGLFW.h"

#include "ncengine/utility/Signal.h"

#include "Graphics/GraphicsEngine/interface/SwapChain.h"

namespace nc::graphics
{
class UIBackend
{
    public:
        UIBackend(Diligent::IRenderDevice& device,
                  const Diligent::SwapChainDesc& swapChainDesc,
                  GLFWwindow* window,
                  std::string_view api,
                  Signal<>& onFontUpdate)
            : m_imguiBackend{device, swapChainDesc, window, api},
              m_fontConnetion{onFontUpdate.Connect(this, &UIBackend::OnFontUpdate)}
        {
        }

        void FrameBegin(Diligent::ISwapChain& swapChain)
        {
            const auto& scDesc = swapChain.GetDesc();
            m_imguiBackend.NewFrame(scDesc.Width, scDesc.Height, scDesc.PreTransform);
        }

        void Render(Diligent::IDeviceContext& context)
        {
            m_imguiBackend.Render(&context);
        }

    private:
        ImGuiImplGLFW m_imguiBackend;
        Connection m_fontConnetion;

        void OnFontUpdate()
        {
            m_imguiBackend.UpdateFontsTexture();
        }
};
} // namespace nc::graphics
