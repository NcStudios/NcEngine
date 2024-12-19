#pragma once

#include "PassManifest.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;

struct ParticlePass
{
    explicit ParticlePass(Diligent::IRenderDevice& device,
                          ShaderFactory& shaderFactory,
                          ShaderBindings& shaderBindings,
                          const PassDesc& passDesc);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    uint32_t colorRTIndex;
    uint32_t depthRTIndex;
};
} // namespace nc::graphics
