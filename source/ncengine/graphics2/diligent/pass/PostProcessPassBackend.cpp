#include "PostProcessPassBackend.h"
#include "graphics2/diligent/resource/PostProcessBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto UpdateBuffer(Diligent::IDeviceContext& context,
                  const nc::PostProcessPassProperties& properties,
                  nc::graphics::PPPassInstanceData& instance)
{
    std::visit(
        [&context, &instance](const auto& unpacked){
            NC_ASSERT(instance.buffer.has_value(), "Pass instance does not have a UniformBuffer");
            instance.buffer->Update(context, unpacked);
        },
        properties
    );
}

void EnableInstance(nc::PostProcessEffectId effectId,
                    nc::graphics::PPPass& pass)
{
    pass.anyEnabled = true;
    auto instance = std::ranges::find(pass.instances, effectId, &nc::graphics::PPPassInstanceData::effectId);
    NC_ASSERT(instance != pass.instances.end(), "Post process pass instance not found");
    instance->enabled = true;
}

void DisableInstance(nc::PostProcessEffectId effectId,
                     nc::graphics::PPPass& pass)
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

auto FindInstance(std::vector<nc::graphics::PPPass>& passes,
                  nc::PostProcessEffectId effectId,
                  nc::PostProcessPass::type passId) -> nc::graphics::PPPassInstanceData&
{
    auto pass = std::ranges::find(passes, passId, &nc::graphics::PPPass::id);
    if (pass != passes.end())
    {
        auto instance = std::ranges::find(pass->instances, effectId, &nc::graphics::PPPassInstanceData::effectId);
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

// todo: want some kind of generic cbuffer that's owned by each pass instance that has properties
// todo: definitely pp resource should use SetVariable to assign new buffer, instead of remapping

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
                                    PostProcessBufferResource& resource)
{
    for (auto& pass : m_passes)
    {
        if (!pass.anyEnabled)
        {
            continue;
        }

        context.SetPipelineState(pass.pso);
        // render target stuff?...
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

            // draw stuff
        }
    }
}
} // namespace nc::graphics
