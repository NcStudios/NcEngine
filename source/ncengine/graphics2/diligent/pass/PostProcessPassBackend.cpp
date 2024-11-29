#include "PostProcessPassBackend.h"
#include "graphics2/diligent/resource/PostProcessBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace
{
auto CopyProperties(const nc::PostProcessPassProperties& properties,
                    nc::graphics::PPPassInstanceData& instance)
{
    std::visit(
        [&instance](const auto& unpacked){
            constexpr auto size = sizeof(unpacked);
            NC_ASSERT(instance.bufferData.size() == size, "PostProcess buffer size mismatch");
            std::memcpy(instance.bufferData.data(), static_cast<const void*>(&unpacked), size);
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
void PostProcessPassBackend::Update(const PostProcessState& postProcessState)
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
        CopyProperties(properties, FindInstance(m_passes, effectId, passId));
    }
}

void PostProcessPassBackend::Render(Diligent::IDeviceContext& context, PostProcessBufferResource& resource)
{
    for (auto& pass : m_passes)
    {
        if (!pass.anyEnabled)
        {
            continue;
        }

        context.SetPipelineState(pass.pso);
        // render target stuff?...
        for (const auto& instance : pass.instances)
        {
            if (!instance.enabled)
            {
                continue;
            }

            resource.Update(context, pass.id, instance.bufferData);
            // draw stuff
        }
    }
}
} // namespace nc::graphics
