#pragma once

#include "graphics2/frontend/subsystem/WireframeRendererState.h"
#include "PassManifest.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;
class WireframeBufferResource;

struct WireframePass
{
    explicit WireframePass(Diligent::IRenderDevice& device,
                            ShaderFactory& shaderFactory,
                            ShaderBindings& shaderBindings,
                            const PassDesc& passDesc,
                            uint32_t numSamples = 1u);
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    WireframeBufferResource* buffer;
    uint32_t colorRTIndex;
    uint32_t depthRTIndex;
};
} // namespace nc::graphics
