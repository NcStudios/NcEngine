#include "UIBackend.h"

#include "ncengine/debug/Profile.h"
#include "imgui.h"
#include "ImGuizmo.h"

namespace nc::graphics
{
UIBackend::UIBackend(Diligent::IRenderDevice& device,
                     const Diligent::SwapChainDesc& swapChainDesc,
                     GLFWwindow* window,
                     Signal<>& onFontUpdate)
    : m_imguiBackend{device, swapChainDesc, window},
      m_fontConnetion{onFontUpdate.Connect(this, &UIBackend::OnFontUpdate)}
{
}

void UIBackend::FrameBegin(Diligent::ISwapChain& swapChain)
{
    NC_PROFILE_SCOPE("UIBackend::FrameBegin", ProfileCategory::Rendering);
    const auto& scDesc = swapChain.GetDesc();
    m_imguiBackend.NewFrame(scDesc.Width, scDesc.Height, scDesc.PreTransform);
    ImGuizmo::BeginFrame();
}

void UIBackend::Render(Diligent::IDeviceContext& context)
{
    NC_PROFILE_SCOPE("UIBackend::Render", ProfileCategory::Rendering);
    m_imguiBackend.Render(&context);
}

void UIBackend::OnFontUpdate()
{
    // Font atlas updates are expressed to the Diligent backend by forcing all
    // cached ImGui textures to be recreated on the next frame.
    // When Diligent provides support for it, we can update this to update a slice of the atlas rather than the entire cache,
    // which is a new feature in ImGui 1.92+.
    m_imguiBackend.InvalidateDeviceObjects();
    m_imguiBackend.CreateDeviceObjects();
}
} // namespace nc::graphics
