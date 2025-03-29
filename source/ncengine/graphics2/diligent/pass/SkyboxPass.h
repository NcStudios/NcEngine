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
class ShaderFactory;

struct SkyboxPass : public Pass
{
    explicit SkyboxPass(Diligent::IRenderDevice& device,
                        ShaderFactory& shaderFactory,
                        ShaderBindings& shaderBindings,
                        const PassManifest& passManifest,
                        const PassDesc& passDesc);
};
} // namespace nc::graphics
