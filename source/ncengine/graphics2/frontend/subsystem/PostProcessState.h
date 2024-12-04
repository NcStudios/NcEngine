#pragma once

#include "ncengine/graphics/PostProcess.h"

#include <vector>

namespace nc::graphics
{
struct PostProcessToggle
{
    PostProcessEffectId effectId = NullPostProcessEffectId;
    PostProcessEffectPasses passes = PostProcessPassFlag::None;
    bool enabled = false;
};

struct PostProcessPropertyUpdate
{
    PostProcessEffectId effectId = NullPostProcessEffectId;
    PostProcessPassFlag::type pass = PostProcessPassFlag::None;
    PostProcessPassProperties properties = EmptyPassProperties{};
};

struct PostProcessState
{
    std::vector<PostProcessToggle> toggledEffects;
    std::vector<PostProcessPropertyUpdate> modifiedProperties;
};
} // namespace nc::graphics
