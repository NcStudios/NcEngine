#include "ImGuiImplGLFW.h"

#include "backends/imgui_impl_glfw.h"

namespace nc::graphics
{
ImGuiImplGLFW::ImGuiImplGLFW(Diligent::IRenderDevice& device,
                             const Diligent::SwapChainDesc& swapChainDesc,
                             GLFWwindow* window)
    : Diligent::ImGuiImplDiligent{Diligent::ImGuiDiligentCreateInfo{
        &device,
        swapChainDesc
      }}
{
    // todo: what about dx12?
    ImGui_ImplGlfw_InitForVulkan(window, true);
}

ImGuiImplGLFW::~ImGuiImplGLFW() noexcept
{
    ImGui_ImplGlfw_Shutdown();
}

void ImGuiImplGLFW::NewFrame(Diligent::Uint32 RenderSurfaceWidth,
                             Diligent::Uint32 RenderSurfaceHeight,
                             Diligent::SURFACE_TRANSFORM SurfacePreTransform)
{
    ImGui_ImplGlfw_NewFrame();
    Diligent::ImGuiImplDiligent::NewFrame(RenderSurfaceWidth, RenderSurfaceHeight, SurfacePreTransform);
}
} // namespace nc::graphics
