#pragma once

#include "Pass.h"
#include "PassManifest.h"
#include "graphics2/diligent/resource/base/DynamicUniformBuffer.h"
#include "graphics2/diligent/resource/ResourceTypes.h"

#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class ShaderBindings;
class ShaderFactory;
class PostProcessColorSinkBufferResource;

// Post process pass data specific to an effect
struct PostProcessPipelineInstance
{
    std::optional<PostProcessPassProperties> properties;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// Post process pass data shared by potentially many effects
struct PostProcessPass : public Pass
{
    PostProcessPass(Diligent::IRenderDevice& device,
                    Diligent::ISwapChain& swapChain,
                    ShaderFactory& shaderFactory,
                    ShaderBindings& shaderBindings,
                    const PassManifest& passManifest,
                    const PassDesc& passDesc);
    std::vector<PostProcessPipelineInstance> instances;
    bool anyEnabled = false;
    uint64_t id = 0;
};
} // namespace nc::graphics
