#pragma once

#include "graphics2/frontend/subsystem/WireframeRendererState.h"
#include "Pass.h"
#include "PassManifest.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;
class WireframeBufferResource;

struct WireframePass : public Pass
{
    explicit WireframePass(Diligent::IRenderDevice& device,
                           const PipelineShaders& shaders,
                           ShaderBindings& shaderBindings,
                           const PassManifest& passManifest,
                           const PassDesc& passDesc,
                           uint32_t numSamples = 1u);
    WireframeBufferResource* buffer;
    bool isMsaa;
};
} // namespace nc::graphics
