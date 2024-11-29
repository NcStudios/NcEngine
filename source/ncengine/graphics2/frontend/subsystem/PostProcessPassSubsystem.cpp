#include "PostProcessSubsystem.h"

#include "ncutility/NcError.h"

#include <concepts>

namespace
{
auto MatchPostProcessPass(nc::PostProcessPass::type pass,
                          const nc::PostProcessPassProperties& properties) -> bool
{
    return std::visit(
        [pass](const auto& unpacked) {
            using T = std::decay_t<decltype(unpacked)>;
            if constexpr (std::same_as<T, nc::OutlinePassProperties>)
                return pass == nc::PostProcessPass::Outline;
            else
                return false;
        },
        properties
    );
}
} // anonymous namespace

namespace nc::graphics
{
PostProcessSubsystem::PostProcessSubsystem()
{
    m_effects.emplace_back(MoebiusEffect);
}

auto PostProcessSubsystem::IsEnabled(PostProcessEffectId effectId) const -> bool
{
    return m_effects.at(effectId).enabled;
}

void PostProcessSubsystem::SetEnabled(PostProcessEffectId effectId, bool enabled)
{
    auto& effect = m_effects.at(effectId);
    effect.enabled = enabled;
    m_toggledEffects.emplace_back(effectId, effect.passes, enabled);
}

auto PostProcessSubsystem::GetProperties(PostProcessEffectId effectId,
                                         PostProcessPass::type pass) const -> const PostProcessPassProperties&
{
    for (const auto& properties : m_effects.at(effectId).properties)
    {
        if (MatchPostProcessPass(pass, properties))
        {
            return properties;
        }
    }

    throw NcError{"Could also return empty state, I suppose"};
}

void PostProcessSubsystem::SetProperties(PostProcessEffectId effectId,
                                         PostProcessPass::type pass,
                                         const PostProcessPassProperties& properties)
{
    auto& effect = m_effects.at(effectId);
    for (auto& existing : effect.properties)
    {
        if (MatchPostProcessPass(pass, existing))
        {
            existing = properties;
            m_modifiedProperties.emplace_back(effectId, pass, properties);
            return;
        }
    }

    throw NcError{"Could NOP, I suppose"};
}

auto PostProcessSubsystem::BuildState() -> PostProcessState
{
    return PostProcessState{
        .toggledEffects = std::move(m_toggledEffects),
        .modifiedProperties = std::move(m_modifiedProperties)
    };
}
} // namespace nc::graphics
