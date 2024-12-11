#pragma once

#include "SkeletalAnimationStorage.h"

namespace nc::graphics
{
class SkeletalAnimationSubsystem
{
    public:
        auto GetStorage() -> SkeletalAnimationStorage& { return m_storage; }

    private:
        SkeletalAnimationStorage m_storage;
};
} // namespace nc::graphics
