#pragma once

#include "ncengine/graphics/Material.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;

struct Pass
{
    explicit Pass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> state,
                  MaterialPassFlag::type passId)
        : pso{std::move(state)},
          id{passId}
    {
    }

    explicit Pass(Diligent::IRenderDevice& device,
                  const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                  MaterialPassFlag::type passId);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPassFlag::type id;
};

auto MakeDefaultGraphicsPipelineCreateInfo(Diligent::ISwapChain& swapChain,
                                           Diligent::IShader& vertexShader,
                                           Diligent::IShader& pixelShader,
                                           std::span<Diligent::IPipelineResourceSignature*> signatures,
                                           std::span<const Diligent::LayoutElement> layoutElements,
                                           std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

// Dummy Toon Pass
auto MakeTestPass(Diligent::IRenderDevice& device,
                  Diligent::ISwapChain& swapChain,
                  ShaderFactory& shaderFactory,
                  Diligent::IPipelineResourceSignature& globalSignature,
                  Diligent::IPipelineResourceSignature& componentSignature,
                  Diligent::IPipelineResourceSignature& materialSignature) -> Pass;

// todo 794 This only creates a Toon pass. Add more as they are implemented.
auto MakePasses(Diligent::IRenderDevice& device,
                Diligent::ISwapChain& swapChain,
                ShaderFactory& shaderFactory,
                ShaderBindings& shaderBindings)-> std::vector<Pass>;
} // namespace nc::graphics
