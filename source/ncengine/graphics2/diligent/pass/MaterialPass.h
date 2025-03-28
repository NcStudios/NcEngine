#pragma once

#include "Pass.h"
#include "PassManifest.h"
#include "ncengine/graphics/Material.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"

#include <span>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;

struct MaterialPass : public Pass
{
    explicit MaterialPass(Diligent::IRenderDevice& device,
                          const PipelineShaders& shaders,
                          ShaderBindings& shaderBindings,
                          const PassManifest& passManifest,
                          const PassDesc& passDesc,
                          uint32_t numSamples);
    MaterialPassFlag::type flag;
    bool isMsaa;
};
} // namespace nc::graphics
