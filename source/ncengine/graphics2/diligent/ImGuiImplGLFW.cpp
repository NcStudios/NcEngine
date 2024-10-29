#include "ImGuiImplGLFW.h"

#include "ncengine/graphics/NcGraphics.h"
#include "ncutility/NcError.h"

#include "backends/imgui_impl_glfw.h"

namespace nc::graphics
{
ImGuiImplGLFW::ImGuiImplGLFW(Diligent::IRenderDevice& device,
                             const Diligent::SwapChainDesc& swapChainDesc,
                             GLFWwindow* window,
                             std::string_view api)
    : Diligent::ImGuiImplDiligent{
        Diligent::ImGuiDiligentCreateInfo{
          &device,
          swapChainDesc
        }
      }
{
    if (api == api::Vulkan)
    {
        ImGui_ImplGlfw_InitForVulkan(window, true);
    }
    else if (api == api::D3D12)
    {
        ImGui_ImplGlfw_InitForOther(window, true);
    }
    else
    {
        throw NcError(fmt::format("Unknown API '{}'", api));
    }
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
