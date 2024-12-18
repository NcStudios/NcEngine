#pragma once

#include "ImGuiImplDiligent.hpp"

#include <string_view>

struct GLFWwindow;

namespace nc::graphics
{
class ImGuiImplGLFW final : public Diligent::ImGuiImplDiligent
{
    public:
        ImGuiImplGLFW(Diligent::IRenderDevice& device,
                      const Diligent::SwapChainDesc& swapChainDesc,
                      GLFWwindow* window);

        ~ImGuiImplGLFW() noexcept;

        void NewFrame(Diligent::Uint32 RenderSurfaceWidth,
                      Diligent::Uint32 RenderSurfaceHeight,
                      Diligent::SURFACE_TRANSFORM SurfacePreTransform) override final;
};
} // namespace nc::graphics
