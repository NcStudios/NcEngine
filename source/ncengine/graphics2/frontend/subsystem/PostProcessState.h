#pragma once

#include "ncengine/graphics/PostProcess.h"

#include <vector>

namespace nc::graphics
{
struct PostProcessToggle
{
    PostProcessEffectId effectId = NullPostProcessEffectId;
    PostProcessEffectPasses passes = PostProcessPass::None;
    bool enabled = false;
};

struct PostProcessPropertyUpdate
{
    PostProcessEffectId effectId = NullPostProcessEffectId;
    PostProcessPass::type pass = PostProcessPass::None;
    PostProcessPassProperties properties = EmptyPassProperties{};
};

struct PostProcessState
{
    std::vector<PostProcessToggle> toggledEffects;
    std::vector<PostProcessPropertyUpdate> modifiedProperties;
};
} // namespace nc::graphics
