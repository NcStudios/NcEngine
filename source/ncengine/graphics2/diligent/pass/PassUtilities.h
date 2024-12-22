#pragma once

#include "MaterialPass.h"
#include "PassTypes.h"
#include "PostProcessPass.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/PostProcessColorSinkBufferResource.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <string_view>
#include <vector>

namespace nc::graphics
{
auto MakePostProcessPassInstances(nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>;
auto ToPassBaseId(const ShaderPaths& shaderPaths, std::string_view name) -> size_t;
auto NoTargets() -> std::vector<uint32_t>;
auto SingleSource(uint32_t target) -> std::vector<uint32_t>;
auto IsOffScreenTarget(uint32_t colorRenderTargetIndex, uint32_t depthRenderTargetIndex) -> bool;
void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       SinkBufferResource& colorSinkBufferResource,
                       SinkBufferResource& depthSinkBufferResource,
                       uint32_t colorRenderTargetIndex,
                       uint32_t depthRenderTargetIndex,
                       bool isMsaa);
void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      SinkBufferResource& colorSinkBufferResource,
                      SinkBufferResource& depthSinkBufferResource,
                      uint32_t colorRenderTargetIndex,
                      uint32_t depthRenderTargetIndex,
                      bool isMsaa);
} // namespace nc::graphics
