#pragma once

#include "PostProcessState.h"

#include "ncengine/graphics/PostProcess.h"

#include <vector>

namespace nc::graphics
{
struct PostProcessEffectState
{
    PostProcessEffectId effectId = NullPostProcessEffectId;
    PostProcessEffectPassFlags passes = PostProcessPassFlag::None;
    std::vector<PostProcessPassProperties> properties = {};
    bool enabled = false;
};

class PostProcessSubsystem
{
    public:
        explicit PostProcessSubsystem();

        auto IsEnabled(PostProcessEffectId effectId) const -> bool;
        void SetEnabled(PostProcessEffectId effectId, bool enabled);
        auto GetProperties(PostProcessEffectId effectId,
                           PostProcessPassFlag::type pass) const -> const PostProcessPassProperties&;
        void SetProperties(PostProcessEffectId effectId,
                           PostProcessPassFlag::type pass,
                           const PostProcessPassProperties& properties);

        auto BuildState() -> PostProcessState;

    private:
        std::vector<PostProcessEffectState> m_effects;
        std::vector<PostProcessToggle> m_toggledEffects;
        std::vector<PostProcessPropertyUpdate> m_modifiedProperties;
};
} // namespace nc::graphics
