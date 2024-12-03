#include "PostProcessPassBackend.h"
#include "graphics2/diligent/pass/PassUtilities.h"
#include "graphics2/diligent/resource/PerPassResourceSignature.h"
#include "graphics2/diligent/resource/PostProcessPropertyBufferResource.h"
#include "graphics2/diligent/resource/ResourceTypes.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
using namespace nc;
using namespace nc::graphics;

auto UpdateBuffer(Diligent::IDeviceContext& context,
                  const PostProcessPassProperties& properties,
                  PostProcessPipelineInstance& instance)
{
    std::visit(
        [&context, &instance](const auto& unpacked){
            NC_ASSERT(instance.buffer.has_value(), "Pass instance does not have a UniformBuffer");
            instance.buffer->Write(context, unpacked);
        },
        properties
    );
}

void EnableInstance(PostProcessEffectId effectId,
                    PostProcessPipeline& pass)
{
    pass.anyEnabled = true;
    auto instance = std::ranges::find(pass.instances, effectId, &PostProcessPipelineInstance::effectId);
    NC_ASSERT(instance != pass.instances.end(), "Post process pass instance not found");
    instance->enabled = true;
}

void DisableInstance(PostProcessEffectId effectId,
                     PostProcessPipeline& pass)
{
    auto anyEnabled = false;
    for (auto& instance : pass.instances)
    {
        if (instance.effectId == effectId)
        {
            instance.enabled = false;
        }
        else
        {
            anyEnabled = anyEnabled || instance.enabled;
        }
    }

    pass.anyEnabled = anyEnabled;
}

auto FindInstance(std::vector<PostProcessPipeline>& passes,
                  PostProcessEffectId effectId,
                  PostProcessPass::type passId) -> PostProcessPipelineInstance&
{
    auto pass = std::ranges::find(passes, passId, &PostProcessPipeline::id);
    if (pass != passes.end())
    {
        auto instance = std::ranges::find(pass->instances, effectId, &PostProcessPipelineInstance::effectId);
        if (instance != pass->instances.end())
        {
            return *instance;
        }
    }

    NC_ASSERT(false, fmt::format(
        "Post process effect/pass mismatch: '{}'/'{}'.",
        effectId,
        passId
    ));

    std::unreachable();
}
} // anonymous namespace

namespace nc::graphics
{
void PostProcessPassBackend::Update(Diligent::IDeviceContext& context,
                                    const PostProcessState& postProcessState)
{
    for (const auto& [effectId, effectPasses, enabled] : postProcessState.toggledEffects)
    {
        for (auto& pass : m_passes)
        {
            if (pass.id & effectPasses)
            {
                enabled ? EnableInstance(effectId, pass) : DisableInstance(effectId, pass);
            }
        }
    }

    for (const auto& [effectId, passId, properties] : postProcessState.modifiedProperties)
    {
        UpdateBuffer(context, properties, FindInstance(m_passes, effectId, passId));
    }
}

void PostProcessPassBackend::Render(Diligent::IDeviceContext& context,
                                    Diligent::ISwapChain& swapChain,
                                    PerPassResourceSignature& perPassResourceSignature,
                                    PostProcessPropertyBufferResource& resource)
{
    constexpr auto drawAttribs = Diligent::DrawAttribs{4, Diligent::DRAW_FLAG_VERIFY_ALL};
    for (auto& pass : m_passes)
    {
        if (!pass.anyEnabled)
        {
            continue;
        }

        BindRenderTarget(context, swapChain, perPassResourceSignature.GetPostProcessSinkBufferResource(), pass.colorRTIndex, pass.depthRTIndex);
        context.SetPipelineState(pass.pso);

        for (auto& instance : pass.instances)
        {
            if (!instance.enabled)
            {
                continue;
            }

            if (instance.buffer.has_value())
            {
                resource.SetVariable(pass.id, instance.buffer->GetBuffer());
            }

            context.Draw(drawAttribs);

            if (IsOffScreenTarget(pass.colorRTIndex, pass.depthRTIndex))
            {
                context.TransitionShaderResources(&perPassResourceSignature.GetResourceBinding());
            }
        }
    }
}
} // namespace nc::graphics
