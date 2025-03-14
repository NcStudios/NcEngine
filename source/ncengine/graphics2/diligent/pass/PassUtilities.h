#pragma once

#include "MaterialPass.h"
#include "PassTypes.h"
#include "PostProcessPass.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/diligent/resource/SinkBufferResource.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineResourceSignature.h"

#include <string_view>
#include <vector>

namespace nc::graphics
{
constexpr auto ClearColor = nc::Vector4{0.050f, 0.050f, 0.050f, 1.0f};
auto ToPassBaseId(const ShaderPaths& shaderPaths, std::string_view name) -> size_t;
auto CreateShaderFromSourceIfInitialized(ShaderFactory& shaderFactory, Diligent::SHADER_TYPE shaderType, const nc::graphics::ShaderPaths& shaderPaths) -> Diligent::RefCntAutoPtr<Diligent::IShader>;
auto GetSinks(const PassManifest& passManifest, const PassDesc& passDesc) -> Sinks;
auto GetSources(const PassManifest& passManifest, const PassDesc& passDesc) -> Sources;
auto HasAnyColorSources(const Sources& sources) -> bool;
auto HasAnyDepthSources(const Sources& sources) -> bool;

void ClearRenderTarget(Diligent::IDeviceContext& context,
                       Diligent::ISwapChain& swapChain,
                       PerPassResourceSignature& perPassResourceSignature,
                       uint32_t colorIndex,
                       uint32_t depthIndex,
                       bool isMsaa);

void ClearPostProcessRenderTarget(Diligent::IDeviceContext& context,
                                  Diligent::ISwapChain& swapChain,
                                  SinkBufferResource& postProcessSinkBufferResource,
                                  uint32_t postProcessIndex);

void ClearPointShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                     Diligent::ISwapChain& swapChain,
                                     SinkBufferResource& shadowMapSinkBufferResource,
                                     uint32_t shadowMapIndex);

void ClearUniShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                   SinkBufferResource& shadowMapSinkBufferResource,
                                   uint32_t shadowMapIndex);

void BindRenderTarget(Diligent::IDeviceContext& context,
                      Diligent::ISwapChain& swapChain,
                      PerPassResourceSignature& perPassResourceSignature,
                      uint32_t colorIndex,
                      uint32_t depthIndex,
                      bool isMsaa);

void BindPostProcessRenderTarget(Diligent::IDeviceContext& context,
                                 Diligent::ISwapChain& swapChain,
                                 SinkBufferResource& postProcessSinkBufferResource,
                                 uint32_t postProcessIndex);

void BindPointShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    SinkBufferResource& shadowMapSinkBufferResource,
                                    uint32_t shadowMapIndex);

void BindUniShadowMapRenderTarget(Diligent::IDeviceContext& context,
                                  SinkBufferResource& shadowMapSinkBufferResource,
                                  uint32_t shadowMapIndex);

auto ToDepthRenderTargetView(Diligent::ISwapChain& swapChain,
                             SinkBufferResource& depthSinkBufferResource,
                             uint32_t index,
                             bool isMsaa) -> Diligent::ITextureView*;

auto ToPostProcessRenderTargetView(Diligent::ISwapChain& swapChain,
                                   SinkBufferResource& postProcessSinkBufferResource,
                                   uint32_t index) -> Diligent::ITextureView*;

auto ToColorRenderTargetView(Diligent::ISwapChain& swapChain,
                             SinkBufferResource& colorSinkBufferResource,
                             uint32_t index,
                             bool isMsaa) -> Diligent::ITextureView*;
} // namespace nc::graphics
