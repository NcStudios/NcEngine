#pragma once

#include "MaterialPass.h"
#include "PassTypes.h"
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

void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       nc::graphics::PostProcessSinkBufferResource& postProcessSinkBufferResource,
                       uint32_t colorRenderTargetIndex,
                       uint32_t depthRenderTargetIndex);

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessSinkBufferResource& postProcessBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex);



auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool;

auto ToPassBaseId(const ShaderPaths& shaderPaths) -> size_t;

auto EmptySource() -> RenderTargets;
auto OffScreenSource(uint32_t colorRTIndex, uint32_t depthRTIndex) -> RenderTargets;
auto SwapChainSink() -> std::pair<uint32_t, uint32_t>;
auto OffScreenSink(uint32_t colorRTIndex, uint32_t depthRTIndex) -> std::pair<uint32_t, uint32_t>;
} // namespace nc::graphics
