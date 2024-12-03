#pragma once

#include "MaterialPass.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/PostProcessSinkBufferResource.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

namespace nc::graphics
{
auto MakeOffScreenMaterialPass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               Diligent::IPipelineResourceSignature& perFrameResourceSignature,
                               PostProcessSinkBufferResource& postProcessBufferResource,
                               std::string_view vertexShaderName,
                               std::string_view pixelShaderName,
                               std::string_view pipelineName) -> MaterialPass;

auto MakeOffScreenPipelineCreateInfo(Diligent::IShader& vertexShader,
                                     Diligent::IShader& pixelShader,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessSinkBufferResource& postProcessBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex);

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool;
} // namespace nc::graphics
