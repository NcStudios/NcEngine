#pragma once

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
class PostProcessSinkBufferResource;

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
    PostProcessPipeline(Diligent::IRenderDevice& device,
                        const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                        std::vector<PostProcessPipelineInstance> instances_,
                        PostProcessPassFlag::type passId,
                        uint32_t colorRTIndex_ = SwapChainColorRTIndex, /** @todo build out support for more than two RT */
                        uint32_t depthRTIndex_ = SwapChainDepthRTIndex,
                        uint32_t renderTargetCount_ = 0u);

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso;
    std::vector<PostProcessPipelineInstance> instances;
    PostProcessPassFlag::type id = PostProcessPassFlag::None;
    uint32_t renderTargetCount = 0u;
    uint32_t colorRTIndex = 0u;
    uint32_t depthRTIndex = 0u;
    bool anyEnabled = false;
};

auto MakePostProcessPasses(Diligent::IDeviceContext& context,
                           Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderBindings& shaderBindings,
                           nc::graphics::PostProcessSinkBufferResource& postProcessSinkBufferResource,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>;
} // namespace nc::graphics
