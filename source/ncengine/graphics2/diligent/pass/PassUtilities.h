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
auto MakeSwapChainPipelineCreateInfo(Diligent::IShader& pixelShader,
                                     Diligent::IShader& vertexShader,
                                     Diligent::ISwapChain& swapChain,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

auto MakeOffScreenPipelineCreateInfo(Diligent::IShader& pixelShader,
                                     Diligent::IShader& vertexShader,
                                     Diligent::ISwapChain& swapChain,
                                     std::span<Diligent::IPipelineResourceSignature*> signatures,
                                     std::span<const Diligent::LayoutElement> layoutElements,
                                     std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

auto MakeOffScreenPostProcessPipelineCreateInfo(Diligent::IShader& pixelShader,
                                                Diligent::IShader& vertexShader,
                                                Diligent::ISwapChain& swapChain,
                                                std::span<Diligent::IPipelineResourceSignature*> signatures,
                                                std::span<const Diligent::LayoutElement> layoutElements,
                                                std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

auto MakeSwapChainPostProcessPipelineCreateInfo(Diligent::IShader& pixelShader,
                                                Diligent::IShader& vertexShader,
                                                Diligent::ISwapChain& swapChain,
                                                std::span<Diligent::IPipelineResourceSignature*> signatures,
                                                std::span<const Diligent::LayoutElement> layoutElements,
                                                std::string_view name) -> Diligent::GraphicsPipelineStateCreateInfo;

auto MakePostProcessPropertyBuffer(Diligent::IDeviceContext& context,
                                   Diligent::IRenderDevice& device,
                                   nc::PostProcessPassFlag::type passId) -> nc::graphics::DynamicUniformBuffer;

auto MakePostProcessPassInstances(Diligent::IDeviceContext& context,
                                  Diligent::IRenderDevice& device,
                                  nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>;

auto MakeOffScreenMaterialPass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               std::span<Diligent::IPipelineResourceSignature*> signatures,
                               PostProcessSinkBufferResource& postProcessSinkBufferResource,
                               std::string_view pixelShaderPath,
                               std::string_view vertexShaderPath,
                               std::string_view pipelineName) -> MaterialPass;

auto MakeSwapChainMaterialPass(Diligent::IRenderDevice& device,
                               Diligent::ISwapChain& swapChain,
                               ShaderFactory& shaderFactory,
                               std::span<Diligent::IPipelineResourceSignature*> signatures,
                               std::string_view pixelShaderPath,
                               std::string_view vertexShaderPath,
                               std::string_view pipelineName) -> MaterialPass;

auto MakeOffScreenPostProcessPass(Diligent::IRenderDevice& device,
                                  Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  ShaderFactory& shaderFactory,
                                  std::span<Diligent::IPipelineResourceSignature*> signatures,
                                  PostProcessSinkBufferResource& postProcessSinkBufferResource,
                                  PostProcessPassFlag::type passId,
                                  std::string_view pixelShaderPath,
                                  std::string_view vertexShaderPath,
                                  std::string_view pipelineName) -> PostProcessPass;

auto MakeSwapChainPostProcessPass(Diligent::IRenderDevice& device,
                                  Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  ShaderFactory& shaderFactory,
                                  std::span<Diligent::IPipelineResourceSignature*> signatures,
                                  PostProcessPassFlag::type passId,
                                  std::string_view pixelShaderPath,
                                  std::string_view vertexShaderPath,
                                  std::string_view pipelineName) -> PostProcessPass;

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessSinkBufferResource& postProcessBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex);

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool;
} // namespace nc::graphics
