#include "PostProcessSubsystem.h"
#include "ncengine/graphics/GraphicsUtility.h"

#include "ncutility/NcError.h"

#include <concepts>

namespace
{
void VerifyPropertyQuery([[maybe_unused]] nc::PostProcessEffectId effectId,
                         [[maybe_unused]] nc::PostProcessPassFlag::type passId)
{
    NC_ASSERT(
        nc::GetCombinedPostProcessEffectPassFlags(effectId) & passId,
        fmt::format("Effect '{}' does not use pass '{}'", effectId, passId)
    );

    NC_ASSERT(
        nc::PassHasProperties(passId),
        fmt::format("Pass '{}' does not have properties", passId)
    );
}

auto BuildEffectStates() -> std::vector<nc::graphics::PostProcessEffectState>
{
    auto effects = std::vector<nc::graphics::PostProcessEffectState>{};
    for (const auto effectId : nc::GetPostProcessEffectIds())
    {
        auto properties = std::vector<nc::PostProcessPassProperties>{};
        auto passes = nc::PostProcessEffectPassFlags{};
        for (const auto pass : nc::GetPostProcessEffectPassFlags(effectId))
        {
            passes |= pass;
            if (nc::PassHasProperties(pass))
            {
                properties.push_back(nc::MakeDefaultPassProperties(pass));
            }
        }

        effects.emplace_back(
            effectId,
            passes,
            std::move(properties),
            false
        );
    }

    return effects;
}

auto MatchPostProcessPass(nc::PostProcessPassFlag::type pass,
                          const nc::PostProcessPassProperties& properties) -> bool
{
    return std::visit(
        [pass](const auto& unpacked) {
            using T = std::decay_t<decltype(unpacked)>;
            if constexpr (std::same_as<T, nc::OutlinePassProperties>)
                return pass == nc::PostProcessPassFlag::Outline;
            else if constexpr (std::same_as<T, nc::GradientPassProperties>)
                return pass == nc::PostProcessPassFlag::Gradient;
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
    : m_effects{BuildEffectStates()}
{
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
                                         PostProcessPassFlag::type pass) const -> const PostProcessPassProperties&
{
    VerifyPropertyQuery(effectId, pass);
    for (const auto& properties : m_effects.at(effectId).properties)
    {
        if (MatchPostProcessPass(pass, properties))
        {
            return properties;
        }
    }

    std::unreachable();
}

void PostProcessSubsystem::SetProperties(PostProcessEffectId effectId,
                                         PostProcessPassFlag::type pass,
                                         const PostProcessPassProperties& properties)
{
    VerifyPropertyQuery(effectId, pass);
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

    std::unreachable();
}

auto PostProcessSubsystem::BuildState() -> PostProcessState
{
    return PostProcessState{
        .toggledEffects = std::move(m_toggledEffects),
        .modifiedProperties = std::move(m_modifiedProperties)
    };
}
} // namespace nc::graphics
