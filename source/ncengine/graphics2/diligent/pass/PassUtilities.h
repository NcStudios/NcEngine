#pragma once

#include "MaterialPass.h"
#include "PostProcessPass.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/PostProcessSinkBufferResource.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <vector>

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

auto MakePostProcessPropertyBuffer(Diligent::IDeviceContext& context,
                                   Diligent::IRenderDevice& device,
                                   nc::PostProcessPass::type passId) -> nc::graphics::DynamicUniformBuffer;

auto MakePostProcessPassInstances(Diligent::IDeviceContext& context,
                                  Diligent::IRenderDevice& device,
                                  nc::PostProcessPass::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>;

auto MakeOffScreenPostProcessPass(Diligent::IRenderDevice& device,
                                  Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  ShaderFactory& shaderFactory,
                                  Diligent::IPipelineResourceSignature& perFrameResourceSignature,
                                  PostProcessSinkBufferResource& postProcessSinkBufferResource,
                                  PostProcessPass::type passId,
                                  std::string_view pixelShaderPath,
                                  std::string_view vertexShaderPath,
                                  std::string_view pipelineName) -> PostProcessPipeline;

auto MakeDefaultPostProcessPass(Diligent::IRenderDevice& device,
                                Diligent::IDeviceContext& context,
                                Diligent::ISwapChain& swapChain,
                                ShaderFactory& shaderFactory,
                                Diligent::IPipelineResourceSignature& perFrameResourceSignature,
                                PostProcessPass::type passId,
                                std::string_view pixelShaderPath,
                                std::string_view vertexShaderPath,
                                std::string_view pipelineName) -> PostProcessPipeline;


auto MakeOffScreenPipelineCreateInfo(Diligent::IShader& vertexShader,
                                     Diligent::IShader& pixelShader,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

auto MakeDefaultPipelineCreateInfo(Diligent::IShader& vertexShader,
                                   Diligent::IShader& pixelShader,
                                   Diligent::ISwapChain& swapChain,
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
