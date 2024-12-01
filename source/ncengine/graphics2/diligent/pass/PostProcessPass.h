#pragma once

#include "graphics2/diligent/resource/base/UniformBuffer.h"
#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class ShaderFactory;

// Post process pass data specific to an effect
struct PostProcessPipelineInstance
{
    std::optional<UniformBuffer> buffer;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// oops, name conflicts w/ flags struct
struct PostProcessPipeline
{
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    std::vector<PostProcessPipelineInstance> instances;
    PostProcessPass::type id = PostProcessPass::None;
    uint32_t renderTargetCount = 0u;
    uint32_t colorRenderTargetIndex = 0u;
    uint32_t depthRenderTargetIndex = 0u;
    bool anyEnabled = false;
};

/** @todo This builds no passes */
auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>;
} // namespace nc::graphics
