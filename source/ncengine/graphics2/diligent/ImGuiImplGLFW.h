#pragma once

#include "ImGuiImplDiligent.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"

#include <memory>

struct GLFWwindow;

namespace nc::graphics
{
class ImGuiImplGLFW final : public Diligent::ImGuiImplDiligent
{
    public:
        // todo: just take dev/sc here instead of ci
        ImGuiImplGLFW(Diligent::IRenderDevice& device,
                      const Diligent::SwapChainDesc& swapChainDesc,
                      GLFWwindow* window);

        ~ImGuiImplGLFW() noexcept;

        void NewFrame(Diligent::Uint32 RenderSurfaceWidth,
                      Diligent::Uint32 RenderSurfaceHeight,
                      Diligent::SURFACE_TRANSFORM SurfacePreTransform) override final;
};
} // namespace nc::graphics
