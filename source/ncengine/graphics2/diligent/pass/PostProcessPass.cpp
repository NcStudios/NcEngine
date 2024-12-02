#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "ncengine/graphics/GraphicsUtility.h"

namespace
{
auto MakeBuffer(Diligent::IDeviceContext& context,
                Diligent::IRenderDevice& device,
                nc::PostProcessPass::type passId) -> nc::graphics::DynamicUniformBuffer
{
    switch (passId)
    {
        case nc::PostProcessPass::Outline:
        {
            return nc::graphics::DynamicUniformBuffer(
                context,
                device,
                nc::graphics::OutlinePassData{},
                "OutlineDataBuffer"
            );
        }
    }

    throw nc::NcError(fmt::format("Unexpected post process pass '{}'", passId));
}

auto MakePassInstances(Diligent::IDeviceContext& context,
                       Diligent::IRenderDevice& device,
                       nc::PostProcessPass::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>
{
    const auto hasProperties = nc::PassHasProperties(passId);
    auto instances = std::vector<nc::graphics::PostProcessPipelineInstance>{};
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        if (!(passId & nc::GetCombinedPostProcessEffectPassFlags(effectId)))
        {
            continue;
        }

        instances.emplace_back(
            hasProperties
                ? std::optional{MakeBuffer(context, device, passId)}
                : std::nullopt,
            effectId,
            false
        );
    }

    return instances;
}
} // anonymous namespace

namespace nc::graphics
{
auto MakePostProcessPasses(Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>
{
    /** @todo I don't have the necessary pieces to flesh this out further */
    // const auto passIds = GetPostProcessPassFlags();
    // auto passes = std::vector<PostProcessPipeline>{};
    // passes.reserve(passIds.size());
    // for (const auto passId : passIds)
    // {
    //     // make PSO...
    //     // set up render target info...
    //     // make instances: MakePassInstances(context, device, passId);
    // }
    //
    // return passes;

    (void)device;
    (void)swapChain;
    (void)shaderFactory;
    return std::vector<PostProcessPipeline>{};
}
} // namespace nc::graphics
