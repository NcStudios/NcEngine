#pragma once

#include "ncengine/graphics/Material.h"
#include "graphics2/diligent/resource/PostProcessSinkBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;

struct MaterialPass
{
    explicit MaterialPass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> state,
                          MaterialPassFlag::type passId,
                          uint32_t colorRTIndex_ = SwapChainColorRTIndex,
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex)
        : pso{std::move(state)},
          id{passId},
          colorRTIndex{colorRTIndex_},
          depthRTIndex{depthRTIndex_}
    {
    }

    explicit MaterialPass(Diligent::IRenderDevice& device,
                          const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                          MaterialPassFlag::type passId,
                          uint32_t colorRTIndex_ = SwapChainColorRTIndex,
                          uint32_t depthRTIndex_ = SwapChainDepthRTIndex);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    MaterialPassFlag::type id;
    uint32_t colorRTIndex;
    uint32_t depthRTIndex;
};

auto MakeOffscreenGraphicsPipelineCreateInfo(Diligent::IShader& vertexShader,
                                             Diligent::IShader& pixelShader,
                                             std::span<Diligent::IPipelineResourceSignature*> signatures,
                                             std::span<const Diligent::LayoutElement> layoutElements,
                                             std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

// Dummy Toon MaterialPass
auto MakeTestPass(Diligent::IRenderDevice& device,
                  Diligent::ISwapChain& swapChain,
                  ShaderFactory& shaderFactory,
                  Diligent::IPipelineResourceSignature& globalSignature,
                  Diligent::IPipelineResourceSignature& componentSignature,
                  Diligent::IPipelineResourceSignature& materialSignature,
                  PostProcessSinkBufferResource& postProcessBufferResource) -> MaterialPass;

// todo 794 This only creates a Toon pass. Add more as they are implemented.
auto MakePasses(Diligent::IRenderDevice& device,
                Diligent::ISwapChain& swapChain,
                ShaderFactory& shaderFactory,
                ShaderBindings& shaderBindings)-> std::vector<MaterialPass>;
} // namespace nc::graphics
