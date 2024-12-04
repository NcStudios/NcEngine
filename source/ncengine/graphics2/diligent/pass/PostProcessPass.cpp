#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/ShaderBindings.h"

#include "ncengine/graphics/GraphicsUtility.h"

namespace nc::graphics
{
PostProcessPipeline::PostProcessPipeline(Diligent::IRenderDevice& device,
                                         const Diligent::GraphicsPipelineStateCreateInfo& createInfo,
                                         std::vector<PostProcessPipelineInstance> instances_,
                                         PostProcessPass::type passId,
                                         uint32_t colorRTIndex_,
                                         uint32_t depthRTIndex_,
                                         uint32_t renderTargetCount_)
    : pso{},
      instances{std::move(instances_)},
      id{passId},
      colorRTIndex{colorRTIndex_},
      depthRTIndex{depthRTIndex_},
      renderTargetCount{renderTargetCount_}
{
    device.CreateGraphicsPipelineState(createInfo, &pso);
    NC_ASSERT(pso, "Failed to create pipeline state object");
}

auto MakePostProcessPasses(Diligent::IDeviceContext& context,
                           Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderBindings& shaderBindings,
                           nc::graphics::PostProcessSinkBufferResource& postProcessSinkBufferResource,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>
{                       
    swapChain;
    shaderFactory;

    const auto passIds = GetPostProcessPassFlags();
    auto passes = std::vector<PostProcessPipeline>{};

    if (passIds.empty())
        return passes;

    const auto passShaderPaths = GetPostProcessPassShaderPaths();
    const auto passNames = GetPostProcessPassNames();
    passes.reserve(passIds.size());
    auto signatures = std::array{&shaderBindings.GetPerFrameSignature().GetResourceSignature(), &shaderBindings.GetPerPassSignature().GetResourceSignature()};

    if (passIds.size() > 1)
    {
        for (auto i = 0u; i < passIds.size()-1; i++)
        {
            passes.emplace_back(MakeOffScreenPostProcessPass(device,
                                                             context,
                                                             swapChain,
                                                             shaderFactory,
                                                             signatures,
                                                             postProcessSinkBufferResource,
                                                             passIds[i],
                                                             passShaderPaths[i].first,
                                                             passShaderPaths[i].second,
                                                             passNames[i]));
        }
    }

    passes.emplace_back(MakeSwapChainPostProcessPass(device,
                                                   context,
                                                   swapChain,
                                                   shaderFactory,
                                                   signatures,
                                                   passIds.back(),
                                                   passShaderPaths.back().first,
                                                   passShaderPaths.back().second,
                                                   passNames.back()));

    return passes;
}
} // namespace nc::graphics
