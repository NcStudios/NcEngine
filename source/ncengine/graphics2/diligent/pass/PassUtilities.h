#pragma once

#include "MaterialPass.h"
#include "PassTypes.h"
#include "PostProcessPass.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/PostProcessColorSinkBufferResource.h"
#include "graphics2/diligent/resource/PostProcessDepthSinkBufferResource.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <vector>

namespace nc::graphics
{
auto MakePostProcessPropertyBuffer(Diligent::IDeviceContext& context,
                                   Diligent::IRenderDevice& device,
                                   nc::PostProcessPassFlag::type passId) -> nc::graphics::DynamicUniformBuffer;

auto MakePostProcessPassInstances(Diligent::IDeviceContext& context,
                                  Diligent::IRenderDevice& device,
                                  nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>;
void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       nc::graphics::PostProcessColorSinkBufferResource& postProcessSinkBufferResource,
                       nc::graphics::PostProcessDepthSinkBufferResource& postProcessDepthSinkBufferResource,
                       uint32_t colorRenderTargetIndex,
                       uint32_t depthRenderTargetIndex);

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      nc::graphics::PostProcessColorSinkBufferResource& postProcessColorSinkBufferResource,
                      nc::graphics::PostProcessDepthSinkBufferResource& postProcessDepthSinkBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex);

auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool;

/** @todo Add pass ID to prevent collisions */
auto ToPassBaseId(const ShaderPaths& shaderPaths) -> size_t;
auto NoTargets() -> std::vector<uint32_t>;
auto SingleSource(uint32_t target) -> std::vector<uint32_t>;
} // namespace nc::graphics
