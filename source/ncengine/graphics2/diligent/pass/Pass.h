#pragma once

#include "ncengine/graphics/Material.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"


#include "graphics2/diligent/ShaderFactory.h"

namespace nc::graphics
{
struct Pass
{
    explicit Pass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> state,
                  MaterialPass::type passId)
        : pso{std::move(state)},
          id{passId}
    {
    }

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPass::type id;
};

auto MakeTestPass(Diligent::IRenderDevice& device,
                  Diligent::ISwapChain& swapChain,
                  ShaderFactory& shaderFactory,
                  std::vector<Diligent::IPipelineResourceSignature*> signatures) -> Pass;
} // namespace nc::graphics
