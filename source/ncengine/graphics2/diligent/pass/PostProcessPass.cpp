#include "PostProcessPass.h"
#include "graphics2/diligent/ShaderFactory.h"

#include "ncengine/graphics/GraphicsUtility.h"

namespace
{
auto MakeBuffer(Diligent::IDeviceContext& context,
                Diligent::IRenderDevice& device,
                nc::PostProcessPassFlag::type passId) -> nc::graphics::DynamicUniformBuffer
{
    switch (passId)
    {
        case nc::PostProcessPassFlag::Outline:
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
                       nc::PostProcessPassFlag::type passId) -> std::vector<nc::graphics::PostProcessPipelineInstance>
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
auto MakePostProcessPasses(Diligent::IDeviceContext& context,
                           Diligent::IRenderDevice& device,
                           Diligent::ISwapChain& swapChain,
                           ShaderFactory& shaderFactory) -> std::vector<PostProcessPipeline>
{
    (void)swapChain;
    (void)shaderFactory;

    const auto passIds = GetPostProcessPassFlags();
    auto passes = std::vector<PostProcessPipeline>{};
    passes.reserve(passIds.size());
    for (const auto passId : passIds)
    {
        /**
         * @todo I don't have all the pieces for this - PSO & render targets are missing. Having the instances
         * here allows backend Update() to be called, but Render() will crash upon enabling a pass. Leaving that
         * call commented out (in NcGraphicsImpl2) until this is finished.
         */
        passes.emplace_back(
            Diligent::RefCntAutoPtr<Diligent::IPipelineState>{},
            MakePassInstances(context, device, passId),
            passId,
            0,
            0,
            0,
            false
        );
    }

    return passes;
}
} // namespace nc::graphics
