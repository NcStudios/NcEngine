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
                  Signal<>& onFontUpdate);

        void FrameBegin(Diligent::ISwapChain& swapChain);
        void Render(Diligent::IDeviceContext& context);

    private:
        ImGuiImplGLFW m_imguiBackend;
        Connection m_fontConnetion;

        void OnFontUpdate();
};
} // namespace nc::graphics
