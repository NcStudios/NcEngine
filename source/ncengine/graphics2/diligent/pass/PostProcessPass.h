#pragma once

#include "graphics2/diligent/resource/base/DynamicUniformBuffer.h"
#include "ncengine/graphics/PostProcess.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"

#include <optional>
#include <vector>

namespace nc::graphics
{
class ShaderFactory;

// Post process pass data specific to an effect
struct PostProcessPipelineInstance
{
    std::optional<DynamicUniformBuffer> buffer;
    PostProcessEffectId effectId = NullPostProcessEffectId;
    bool enabled = false;
};

// Post process pass data shared by potentially many effects
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
auto MakePostProcessPasses(Diligent::IDeviceContext& context,
                           Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>;
} // namespace nc::graphics
