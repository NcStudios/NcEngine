#pragma once

#include "Pass.h"
#include "PassManifest.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;

struct ParticlePass : public Pass
{
    explicit ParticlePass(Diligent::IRenderDevice& device,
                          ShaderFactory& shaderFactory,
                          ShaderBindings& shaderBindings,
                          const PassManifest& passManifest,
                          const PassDesc& passDesc,
                          uint32_t numSamples);
    bool isMsaa;
};
} // namespace nc::graphics
